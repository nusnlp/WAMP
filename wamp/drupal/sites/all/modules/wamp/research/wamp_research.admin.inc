<?php
// @author Chan Hong Guan chanhg -at- comp.nus.edu.sg
// $Id: wamp_research.admin.inc 171 2009-08-19 03:04:35Z chanhg $

/**
 * @ingroup wamp_research
 * @{
 */

/**
 * Build an overview form of imported xml records.
 *
 * Loads all records and builds an overview form with links for processing the
 * imported xml or deleting the record.
 *
 * @ingroup forms
 * @see theme_wamp_research_overview_form()
 * @see _wamp_research_overview_record_links()
 */
function wamp_research_overview_form() {

    $header = array(
        array('data' => t('Filename'), 'field' => 'original_filename'),
        array('data' => t('Uploaded Date'), 'field' => 'uploaded_date'),
        array('data' => t('Uploader')),
        t('Operations')
    );

    $sql = "select import_id, original_filename, uploaded_date, uid
                FROM {wamp_essay_imported} i";
    $sql .= tablesort_sql($header);
    $query_count = 'select count(import_id) FROM {wamp_essay_imported} i';
    $result = pager_query($sql, 50, 0, $query_count);
    $destination = drupal_get_destination();

	$import_count = 0;
    while ($import = db_fetch_object($result)) {

		$imports[$import->import_id] = '';
        $form['filename'][$import->import_id] = array('#value' => $import->original_filename);
        $form['uploaded_date'][$import->import_id] = array('#value' => $import->uploaded_date);

        $uploader = user_load(array('uid' => $import->uid));

        $form['uid'][$import->import_id] = array('#value' => $uploader->name);

        $form['operations'][$import->import_id] = array(
            '#type' => 'markup',
            '#value' => _wamp_research_overview_record_links($import),
        );

		$import_count++;
    }

	$form['imports'] = array(
		'#type' => 'checkboxes',
		'#options' => $imports
	);

	if ($import_count > 0) {
		$form['submit']  = array(
			'#type' => 'submit',
			'#value' => t('Remove selected imports'),
		);
	}

    return $form;
}

/**
 * General submit handler for the list students in module group form.
 *
 * Checked students are removed from the group
 *
 * @ingroup formapi
 * @see wamp_module_group_list_students_form()
 */
function wamp_research_overview_form_submit($form, &$form_state) {
	$imports = array_filter($form_state['values']['imports']);
	$keys = array_keys($imports);
	$record = array();
    
    //The section below is commented out to remove module information.
	// $record['modulegroup_id'] = $form_state['values']['modulegroup_id'];

	$sql = "select original_filename, uploaded_date
                FROM {wamp_essay_imported} i WHERE import_id=%d";

	for ($i=0;$i<sizeof($keys);$i++) {
		$import_id = $keys[$i];
		$result = db_query($sql, (int) $import_id);
		if ($record = db_fetch_object($result)){
			$original_filename = $record->original_filename;
			$uploaded_date = $record->uploaded_date;
		}

		db_query('DELETE FROM {wamp_essay_imported} WHERE import_id=%d', $import_id);

		$uploaded_filename = 'essay.'.strtotime($uploaded_date).'.'.$original_filename;
		$filename = getcwd().'/files/'.$uploaded_filename;
		file_delete($filename);
	}

	drupal_set_message(t('Imports removed successfully'));

	$form_state['redirect'] = 'admin/wamp/wamp_research';
}

/**
 * Build the process and delete links for an imported xml record.
 *
 * @see wamp_research_overview_form()
 */
function _wamp_research_overview_record_links($record) {
    $links['process'] = array(
        'title' => t('Process'),
        'href' => "admin/wamp/wamp_research/".$record->import_id."/process",
        'html' => TRUE,
    );

    $links['delete'] = array(
        'title' => t('Delete'),
        'href' => "admin/wamp/wamp_research/".$record->import_id."/delete",
        'html' => TRUE,
    );

    return theme('links', $links);
}

/**
 * Theme wamp research overview form for imported xml records.
 *
 * @ingroup themeable
 * @see wamp_research_overview_form()
 */
function theme_wamp_research_overview_form($form) {

    $header = array(
		theme('table_select_header_cell'),
        array('data' => t('Filename'), 'field' => 'original_filename'),
        array('data' => t('Uploaded Date'), 'field' => 'uploaded_date'),
        array('data' => t('Uploader')),
        t('Operations')
    );


    if (isset($form['filename']) && is_array($form['filename'])) {
        foreach (element_children($form['filename']) as $key) {
        //       print_r($form['modulecode']);
            $rows[] = array(
				drupal_render($form['imports'][$key]),
                drupal_render($form['filename'][$key]),
                drupal_render($form['uploaded_date'][$key]),
                drupal_render($form['uid'][$key]),
                drupal_render($form['operations'][$key]),
            );
        }
    }
    else {
        $rows[] = array(array('data' => t('No imported xml files available.'), 'colspan' => '5'));
    }

    $output .= theme('table', $header, $rows);
    if ($form['pager']['#value']) {
        $output .= drupal_render($form['pager']);
    }

    $output .= drupal_render($form);

    return $output;
}

/**
 * Build a form for importing xml files with a submit and cancel button. The
 * Cancel button redirects to the research overview form.
 *
 * @ingroup forms
 * @see wamp_research_import_form_submit()
 */
function wamp_research_import_form() {

    $form['#attributes'] = array('enctype' => "multipart/form-data");

    $form['file'] = array(
        '#type' => 'file',
        '#title' => t('File to Upload'),
    );

    $form['buttons']['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Upload',
    );
    // $form['buttons']['cancel'] = array(
    //     '#type' => 'markup',
    //     '#value' => '<input type="button" onclick="javascript:location.href=\'history.back();\'" value="Cancel">',
    // );
    $form['cancel'] = array(
        '#type' => 'markup',
        '#value' => t('<input type="button" id="research_form_cancel_btn" value="Cancel">'),
    );
    drupal_add_js(drupal_get_path('module', 'wamp_research').'/js/'.'research_form_cancel_event_handler.js');

    return $form;
}

/**
 * General submit handler for xml file import form.
 *
 * Copies file from tmp location to designated location
 * and inserts an entry into database for later post-processing
 *
 * @ingroup formapi
 * @see wamp_research_import_form()
 */
function wamp_research_import_form_submit($form, &$form_state) {

    global $user;

    $validators = array(
        'file_validate_size' => array(4 * 1024 * 1024 * 1024),
    );

    if ($file = file_save_upload('file', $validators)) {
    // All that validation is taken care of... but image was saved using
    // file_save_upload() and was added to the files table as a
    // temporary file. We'll make a copy and let the garbage collector
    // delete the original upload.
        if ($file->filemime == 'application/xml') {
            $time = time();
            $destination = getcwd().'/'.'files/essay.'.$time.'.'.$file->filename;
            $original_filename = $file->filename;
            file_copy($file,  $destination, FILE_EXISTS_REPLACE);
            file_delete($file->destination);

            if (file_exists($destination)) {

                $uploaded_file = file_get_contents($destination);
                // The section below is added to convert all curly/smart quotes to regular quotes.
                $uploaded_file = convert_curly_quotes($uploaded_file);

                $sql = "INSERT INTO {wamp_essay_imported} (xml, uploaded_date,uid,original_filename)
					VALUES('%s',FROM_UNIXTIME(%d),%d,'%s')";
                $result = db_query($sql, array($uploaded_file,$time,$user->uid,$original_filename));


                drupal_set_message('File uploaded successfully');
            }
            else form_set_error('', 'File uploading failed');

        }
        else form_set_error('file', 'Invalid extension. Only XML files are allowed.');
    }
}


/**
 * Build a form for postprocessing imported xml files
 *
 * There are 3 configuration options.
 * 1) Disable Created User Accounts
 * 2) Test Run
 * 3) Use Title in XML File
 *
 * @ingroup forms
 * @see wamp_research_import_process_form_submit()
 */
function wamp_research_import_process_form(&$form_state, $record=array()) {

    $form['import_id'] = array(
        '#type' => 'value',
        '#value' => $record['import_id'],
    );

    $form['title'] = array(
        '#type' => 'textfield',
        '#title' => 'Standardised Title of Essay',
    );

    //The section below is commented out to remove configuration option for idabling created user accounts, since no user account (i.e. student) is created via importing essays anymore.
    //
    // $form['disable_created_users'] = array(
    //     '#type' => 'checkbox',
    //     '#title' => 'Disable Created User Accounts',
    //     '#default_value' => 1,
    // );

	$form['test_run'] = array(
		'#type' => 'checkbox',
        '#title' => 'Test Run Only',
        '#default_value' => 1,
	);

	$form['use_title_in_xml_file'] = array(
		'#type' => 'checkbox',
        '#title' => 'Use Title in XML File',
        '#default_value' => 1,
	);

    //	$form['assign_users_to_role_student'] = array(
    //		'#type' => 'checkbox',
    //		'#title' => 'Assign New User to \'student\' Role',
    //	);

    $form['buttons']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Process'),
    );

    // $form['buttons']['cancel'] = array(
    //     '#type' => 'markup',
    //     '#value' => '<input type="button" value="Cancel" onclick="javascript:history.back()">',
    // );

    return $form;
}

/**
 * Submit handler for postprocessing imported xml files.
 *
 * The xml is loaded from the database and processed in the following steps
 * (1) Module is created if it does not exist (checked against module code)
 * (2) Essay template is created if it does not exist
 * (3) Users are created if they do not exist (checked against login username)
 * (4) Essay is created and assigned to user
 *
 * There are 3 configuration options.
 * 1) Disable Created User Accounts
 * 2) Test Run
 * 3) Use Title in XML File
 *
 * If option 1 is checked, user accounts that are created are disabled.
 * If option 2 is checked, no actual data will be inserted into the database.
 * If option 3 is checked, the essay title from the xml file will be used. The
 * title input into the textfield will be ignored.
 *
 * @ingroup formapi
 * @see wamp_research_import_form()
 */
function wamp_research_import_process_form_submit($form, &$form_state) {

	//variable init
    global $user;
    $record = $form_state['values'];
    $title = $record['title'];
    $import_id = $record['import_id'];
	$test_run = $record['test_run'];
	$use_title_in_xml_file = $record['use_title_in_xml_file'];

    //The section below is commented out to remove configuration option for idabling created user accounts, since no user account (i.e. student) is created via importing essays anymore.
    //
    // $disable_created = $record['disable_created_users'];
    // if ($disable_created == 1) $disable_created = 0;
    // else $disable_created = 1;

	//define variables for checking progress
    //The section below is commented out to remove module information.
	//
    // $module_created = '';
	// $group_created = '';
    //The section below is commented out to remove student information.
	// $student_assigned = '';

	//define constants
    //The section below is commented out to remove module information.
	//
    // define(MODULE_CREATED, 100);
	// define(GROUP_CREATED, 200);
	define(ASSIGNMENT_CREATED, 300);
	//The section below is commented out to remove student information.
    // define(STUDENT_ASSIGNED, 400);

	//display configuration options
	drupal_set_message('#### CONFIGURATION OPTIONS ####');
    
    //The section below is commented out to remove configuration option for idabling created user accounts, since no user account (i.e. student) is created via importing essays anymore.
	// if ($disable_created == 0)
	// 	drupal_set_message('Disable Created Users = Yes');
	// else drupal_set_message('Disable Created Users = No');

	if ($test_run == 1)
		drupal_set_message('Test Run = Yes');
	else drupal_set_message('Test Run = No');
	if ($use_title_in_xml_file == 1)
		drupal_set_message('Use Title in XML file = Yes');
	else {
		drupal_set_message('Use Title in XML file = No');
		drupal_set_message('Standardized Title = ' . $title);
	}	
	drupal_set_message('#### END CONFIGURATION OPTIONS ####');

	//retrieve uploaded xml document from database
    $sql = "select xml FROM {wamp_essay_imported} i WHERE import_id=%d";
    $xml = db_result(db_query($sql, $import_id));

    //initialise xml parser
    $imports = new SimpleXMLElement($xml);
    
    //The section below is commented out to remove essay template information.
    // //step 3 create essay template if essay template does not exist
    // $assignments = $imports->assignment;
    // $num_assignments = sizeof($assignments);
    // $titles = array($num_assignments);
    // drupal_set_message("Number of assignments detected = " . $num_assignments);
    // for ($k=0;$k<$num_assignments;$k++) {
    //     $assignment = $assignments[$k];
    //     if ($use_title_in_xml_file) $title = $assignment['title'];
    //     drupal_set_message('Title = ' . $title);
        
    //     //The section below is commented out to remove module information.
    //     // $sql_findessaytemplate = "SELECT essaytemplate_id FROM {wamp_essay_template}
    //     //     WHERE module_id=%d AND title='%s' AND display_flag=1";
    //     // $essaytemplate_id = db_result(db_query($sql_findessaytemplate, array($module_id_in_db, $title)));
    //     $sql_findessaytemplate = "SELECT essaytemplate_id FROM {wamp_essay_template}
    //         WHERE title='%s' AND display_flag=1";
    //     $essaytemplate_id = db_result(db_query($sql_findessaytemplate, array($title)));

    //     if (empty($essaytemplate_id)) {
    //         $essay_template = array();
    //         $essay_template['title'] = $title;
    //         $essay_template['description'] = trim($assignment->description);
    //         //The section below is commented out to remove module information.
    //         // $essay_template['module_id'] = $module_id_in_db;
    //         $essay_template['annotator_id'] = $user->uid;
    //         $essay_template['display_flag'] = 1;
    //         if ($test_run != 1) {
    //             $saved_essay_template = wamp_essay_record_save($essay_template);
    //             if ($saved_essay_template != FALSE) {
    //                 //The section below is commented out to remove module information.
    //                 // $essaytemplate_id = db_result(db_query($sql_findessaytemplate, array($module_id_in_db, $title)));
    //                 $essaytemplate_id = db_result(db_query($sql_findessaytemplate, array($title)));
    //             }
    //             if (!empty($essaytemplate_id)) {
    //                 $titles[$k] = ASSIGNMENT_CREATED;
    //                 drupal_set_message('Essay Template ID = ' . $essaytemplate_id);
    //             }
    //         }
    //         else $titles[$k] = ASSIGNMENT_CREATED;
    //     }
    //     else {
    //         $titles[$k] = ASSIGNMENT_CREATED;
    //         drupal_set_message('Essay Template ID = ' . $essaytemplate_id);
    //     }

    //     //The section below is commented out to remove module information.
    //     // if ($module_created == MODULE_CREATED && $group_created == GROUP_CREATED && $titles[$k] == ASSIGNMENT_CREATED) {
    //     if ($titles[$k] == ASSIGNMENT_CREATED) {

    //         //initialise <instance> in the xml
    //         $instances = $assignment->instance;
    //         $num_instances = sizeof($instances);
    //         //The section below is commented out to remove student information.
    //         //
    //         // $num_correct_matric = 0;
    //         // $num_incorrect_matric = 0;

    //         drupal_set_message("Number of essays detected = " . $num_instances);

    //         for ($l=0;$l<$num_instances;$l++) {

    //             $instance = $instances[$l];
    //             //The section below is commented out to remove student information.
    //             //
    //             // $nationality = $instance['nationality'];
    //             // $language = $instance->language;
    //             // $matric = $instance['matric'];
                
    //             if (isset($instance['nid'])) {
    //                 $nid = $instance['nid'];
    //             } else {
    //                 $nid = 0;
    //             }

    //             //The section below is commented out to remove student information.
    //             //
    //             // $matric = check_plain($matric);
    //             // $matric = strtoupper($matric);
    //             // if ($instance['name'] != '') $name = $instance['name'];
    //             // else $name = "$matric Imported User";

    //             //The section below is commented out to remove student information.
    //             //
    //             // //check undergrad matric number
    //             // //$matric_check = preg_match("/^(U|NT|HT)[0-9]{6}[A-Z]$/", $matric);
    //             // $matric_check = preg_match("/^(A[0-9]|U|NT|HT)[0-9]{6}[A-Z]$/", $matric);
    //             // if ($matric_check > 0) {

    //             // //search for user with login name = matric
    //             //     $sql_finduser = "SELECT uid FROM {users} WHERE name='%s'";
    //             //     $uid = db_result(db_query($sql_finduser, array($matric)));
    //             //     $role_id = db_result(db_query("SELECT rid FROM {role} WHERE name='%s' LIMIT 1", array('student')));
    //             //     $roles = array($role_id => 'student');

    //             //     if (!empty($uid)) {
    //             //         drupal_set_message("found user $matric");
    //             //         $num_correct_matric++;

    //             //         $saved_user = user_save(user_load(array('name' => $matric)),
    //             //                 array(
    //             //                 'name' => $matric,
    //             //                 'status' =>  $disable_created,
    //             //                 'roles' => $roles));

    //             //         drupal_set_message('name = ' . $name);
    //             //         $fields = array(
    //             //             'profile_pers_matricnum' => $matric,
    //             //             'profile_pers_name' => $name);
    //             //         profile_save_profile($fields,
    //             //             user_load(array('name' => $matric)),
    //             //             'Personal Information');
    //             //     }
    //             //     else {
    //             //     //step 4 create user if user does not exist
    //             //         $newuser = new stdClass();
    //             //         $newuser->uid = 0;								

    //             //         if ($test_run != 1) {
    //             //                 $saved_user = user_save($newuser,array('name' => $matric,'status' =>  $disable_created, 'roles' => $roles));
    //             //             $fields = array(
    //             //                 'profile_pers_matricnum' => $matric,
    //             //                 'profile_pers_name' => $name);
    //             //             profile_save_profile($fields,
    //             //                 $saved_user,
    //             //                 'Personal Information');
    //             //             drupal_set_message("created user $matric");
    //             //         }
    //             //         $num_correct_matric++;
    //             //     }
    //             //     $author = user_load(array('name' => $matric));

    //             //     //additional info like nationality and languages etc
    //             //     if ($test_run != 1) {
    //             //         $useraddinfo = db_result(db_query('SELECT COUNT(student_id) FROM {wamp_student} WHERE student_id=%d',
    //             //             array($author->uid)));
    //             //         if (empty($useraddinfo) || $useraddinfo == 0) {
    //             //             $sql_useraddinfo = "INSERT INTO {wamp_student} (student_id, attribute, value) VALUES(%d,'%s','%s')";
    //             //             db_query($sql_useraddinfo, array($author->uid, 'nationality', $nationality));


    //             //             $langs = $language->lang;
    //             //             drupal_set_message('Number of languages detected = ' . count($langs));
    //             //             for ($m=0;$m<count($langs); $m++) {
    //             //                 $lang = $langs[$m];
    //             //                 db_query($sql_useraddinfo, array($author->uid, 'language-'.($m+1), $lang['name']));
    //             //             }
    //             //         }
    //             //         else {
    //             //             $sql_update_useraddinfo = "UPDATE {wamp_student} SET value='%s' WHERE student_id=%d AND attribute='%s'";
    //             //             $sql_select_useraddinfo = 'SELECT value FROM {wamp_student} WHERE student_id=%d AND attribute=\'%s\'';
    //             //             $sql_insert_useraddinfo = "INSERT INTO {wamp_student} (student_id, attribute, value) VALUES(%d,'%s','%s')";

    //             //             $useraddinfo_nationality = db_result(db_query($sql_select_useraddinfo,array($author->uid, 'nationality')));
    //             //             if (!empty($useraddinfo_nationality) || $useraddinfo_nationality == '')
    //             //                 db_query($sql_update_useraddinfo, array($nationality, $author->uid, 'nationality'));
    //             //             else db_query($sql_insert_useraddinfo, array($author->uid, 'nationality', $nationality));

    //             //             $langs = $language->lang;
    //             //             for ($m=0;$m<count($langs); $m++) {
    //             //                 $lang = $langs[$m];
    //             //                 $useraddinfo_lang = db_result(
    //             //                     db_query($sql_select_useraddinfo, array($author->uid, 'language-'.($m+1))));
    //             //                 if (!empty($useraddinfo_lang) || $useraddinfo_lang == '')
    //             //                     db_query($sql_update_useraddinfo, array($lang['name'], $author->uid, 'language-'.($k+1)));
    //             //                 else db_query($sql_insert_useraddinfo, array($author->uid, 'language-'.($m+1), $lang['name']));
    //             //             }
    //             //         }
    //             //     }

    //             //     //step 5 create and save essay
    //             //     $essay_contents = $instance->context;
    //             //     print_r($instance->context);
    //             //     $essay = new stdClass();
    //             //     $essay->is_new = TRUE;
    //             //     $essay->uid = $author->uid;
    //             //     $essay->created = time();
    //             //     $essay->type = 'wamp_essay';
    //             //     $essay->essaytemplate_id = $essaytemplate_id;
    //             //     $essay->body = $essay_contents;
    //             //     $essay->format = 1;
    //             //     $essay->title = $title;
    //             //     $essay->op = t('Submit to Tutor');
    //             //     $essay->name = $author->name;
    //             //     $essay->status = 1;
    //             //     $essay->comment = 2;
    //             //     $essay->validated = 1;
    //             //     $essay->revision = 1;
    //             //     $essay->log = t('Created New Draft');
    //             //     drupal_set_message("NID = " . $nid);
    //             //     if ($essay->is_new && $test_run != 1) {
    //             //         _node_save_revision($essay, $author->uid);
    //             //         print_r($node);
    //             //         drupal_write_record('node', $essay);
    //             //         $vid = $nid;
    //             //         if ($nid == 0) { // chrhad
    //             //             $nid = $essay->nid; // follow natural order
    //             //             $vid = $essay->vid; // follow natural order
    //             //         }
    //             //         db_query('UPDATE {node} SET nid = %d, vid = %d WHERE nid = %d', $nid, $vid, $essay->nid);
    //             //         db_query('UPDATE {node_revisions} SET nid = %d, vid = %d WHERE vid = %d', $nid, $vid, $essay->vid);
    //             //         $op = 'insert';

    //             //         $sql = "INSERT INTO {wamp_essay_revision} (nid,vid,essay_id,essaytemplate_id,student_id, status)
    //             //         VALUES(%d, %d, %d, %d, %d, %d)";
    //             //         db_query($sql, array($nid, $vid, $nid, $essay->essaytemplate_id, $essay->uid, 2));
    //             //     }

    //             //     //The section below is commented out to remove module information.
    //             //     //
    //             //     // //step 6 assign student to group
    //             //     // $sql_findstudent = "SELECT modulegroupstudent_id FROM {wamp_module_group_student}
    //             //     //     WHERE modulegroup_id = %d AND student_id = %d";
    //             //     // $modulegroupstudent_id_in_db = db_result(db_query($sql_findstudent, array($modulegroup_id_in_db, $author->uid)));
    //             //     // drupal_set_message('module group student id =' . $modulegroupstudent_id_in_db);
    //             //     // if (empty($modulegroupstudent_id_in_db)) {
    //             //     //     $sql_insertstudent = "INSERT INTO {wamp_module_group_student} (modulegroup_id, student_id)
    //             //     //         VALUES(%d, %d)";
    //             //     //     if ($test_run != 1) {
    //             //     //         db_query($sql_insertstudent, array($modulegroup_id_in_db, $author->uid));
    //             //     //         $modulegroupstudent_id_in_db = db_result(db_query($sql_findstudent, array($modulegroup_id_in_db, $author->uid)));
    //             //     //         if (!empty($modulegroupstudent_id_in_db)) {
    //             //     //             $student_assigned = STUDENT_ASSIGNED;
    //             //     //             drupal_set_message('Module Group Student ID = ' . $modulegroupstudent_id_in_db);
    //             //     //         }
    //             //     //     }
    //             //     //     else $student_assigned = STUDENT_ASSIGNED;
    //             //     // }
    //             //     // else {
    //             //     //     $student_assigned = STUDENT_ASSIGNED;
    //             //     //     drupal_set_message('Module Group Student ID = ' . $modulegroupstudent_id_in_db);
    //             //     // }

    //             // }
    //             // else {
    //             //     $num_incorrect_matric++;
    //             //     drupal_set_message("incorrect matric number = $matric");
    //             // }
                
    //             /**
    //              * The section below is commented out to remove student information.
    //              * 
    //             $sql_finduser = "SELECT uid FROM {users} WHERE name='%s'";
    //             $uid = db_result(db_query($sql_finduser, array($matric)));
    //             $role_id = db_result(db_query("SELECT rid FROM {role} WHERE name='%s' LIMIT 1", array('student')));
    //             $roles = array($role_id => 'student');

    //             if (!empty($uid)) {
    //                 drupal_set_message("found user $matric");
    //                 $num_correct_matric++;

    //                 $saved_user = user_save(user_load(array('name' => $matric)),
    //                         array(
    //                         'name' => $matric,
    //                         'status' =>  $disable_created,
    //                         'roles' => $roles));

    //                 drupal_set_message('name = ' . $name);
    //                 $fields = array(
    //                     'profile_pers_matricnum' => $matric,
    //                     'profile_pers_name' => $name);
    //                 profile_save_profile($fields,
    //                     user_load(array('name' => $matric)),
    //                     'Personal Information');
    //             }
    //             else {
    //             //step 4 create user if user does not exist
    //                 $newuser = new stdClass();
    //                 $newuser->uid = 0;								

    //                 if ($test_run != 1) {
    //                         $saved_user = user_save($newuser,array('name' => $matric,'status' =>  $disable_created, 'roles' => $roles));
    //                     $fields = array(
    //                         'profile_pers_matricnum' => $matric,
    //                         'profile_pers_name' => $name);
    //                     profile_save_profile($fields,
    //                         $saved_user,
    //                         'Personal Information');
    //                     drupal_set_message("created user $matric");
    //                 }
    //                 $num_correct_matric++;
    //             }
    //             $author = user_load(array('name' => $matric));

    //             //additional info like nationality and languages etc
    //             if ($test_run != 1) {
    //                 $useraddinfo = db_result(db_query('SELECT COUNT(student_id) FROM {wamp_student} WHERE student_id=%d',
    //                     array($author->uid)));
    //                 if (empty($useraddinfo) || $useraddinfo == 0) {
    //                     $sql_useraddinfo = "INSERT INTO {wamp_student} (student_id, attribute, value) VALUES(%d,'%s','%s')";
    //                     db_query($sql_useraddinfo, array($author->uid, 'nationality', $nationality));


    //                     $langs = $language->lang;
    //                     drupal_set_message('Number of languages detected = ' . count($langs));
    //                     for ($m=0;$m<count($langs); $m++) {
    //                         $lang = $langs[$m];
    //                         db_query($sql_useraddinfo, array($author->uid, 'language-'.($m+1), $lang['name']));
    //                     }
    //                 }
    //                 else {
    //                     $sql_update_useraddinfo = "UPDATE {wamp_student} SET value='%s' WHERE student_id=%d AND attribute='%s'";
    //                     $sql_select_useraddinfo = 'SELECT value FROM {wamp_student} WHERE student_id=%d AND attribute=\'%s\'';
    //                     $sql_insert_useraddinfo = "INSERT INTO {wamp_student} (student_id, attribute, value) VALUES(%d,'%s','%s')";

    //                     $useraddinfo_nationality = db_result(db_query($sql_select_useraddinfo,array($author->uid, 'nationality')));
    //                     if (!empty($useraddinfo_nationality) || $useraddinfo_nationality == '')
    //                         db_query($sql_update_useraddinfo, array($nationality, $author->uid, 'nationality'));
    //                     else db_query($sql_insert_useraddinfo, array($author->uid, 'nationality', $nationality));

    //                     $langs = $language->lang;
    //                     for ($m=0;$m<count($langs); $m++) {
    //                         $lang = $langs[$m];
    //                         $useraddinfo_lang = db_result(
    //                             db_query($sql_select_useraddinfo, array($author->uid, 'language-'.($m+1))));
    //                         if (!empty($useraddinfo_lang) || $useraddinfo_lang == '')
    //                             db_query($sql_update_useraddinfo, array($lang['name'], $author->uid, 'language-'.($k+1)));
    //                         else db_query($sql_insert_useraddinfo, array($author->uid, 'language-'.($m+1), $lang['name']));
    //                     }
    //                 }
    //             }
    //              */

    //             //step 5 create and save essay
    //             $essay_contents = $instance->context;
    //             print_r($instance->context);
    //             $essay = new stdClass();
    //             $essay->is_new = TRUE;
                
    //             //The section below is commented out to remove student information.
    //             //Save the uid of the admin instead of the student.
    //             // $essay->uid = $author->uid;
    //             $essay->uid = $user->uid;

    //             $essay->created = time();
    //             $essay->type = 'wamp_essay';
    //             $essay->essaytemplate_id = $essaytemplate_id;
    //             $essay->body = $essay_contents;
    //             $essay->format = 1;
    //             $essay->title = $title;
    //             $essay->op = t('Submit to Tutor');
    //             //The section below is commented out to remove student information.
    //             // $essay->name = $author->name;
    //             $essay->status = 1;
    //             $essay->comment = 2;
    //             $essay->validated = 1;
    //             $essay->revision = 1;
    //             $essay->log = t('Created New Draft');
    //             drupal_set_message("NID = " . $nid);
    //             if ($essay->is_new && $test_run != 1) {
                    
    //                 //The section below is commented out to remove student information.
    //                 //Save the uid of the admin instead of the student.
    //                 // _node_save_revision($essay, $author->uid);
    //                 _node_save_revision($essay, $user->uid);

    //                 print_r($node);
    //                 drupal_write_record('node', $essay);
    //                 $vid = $nid;
    //                 if ($nid == 0) { // chrhad
    //                     $nid = $essay->nid; // follow natural order
    //                     $vid = $essay->vid; // follow natural order
    //                 }
    //                 db_query('UPDATE {node} SET nid = %d, vid = %d WHERE nid = %d', $nid, $vid, $essay->nid);
    //                 db_query('UPDATE {node_revisions} SET nid = %d, vid = %d WHERE vid = %d', $nid, $vid, $essay->vid);
    //                 $op = 'insert';

    //                 //The section below is commented out to remove student information.
    //                 //
    //                 // $sql = "INSERT INTO {wamp_essay_revision} (nid,vid,essay_id,essaytemplate_id,student_id, status)
    //                 // VALUES(%d, %d, %d, %d, %d, %d)";
    //                 // db_query($sql, array($nid, $vid, $nid, $essay->essaytemplate_id, $essay->uid, 2));
    //                 $sql = "INSERT INTO {wamp_essay_revision} (nid, vid,essay_id, essaytemplate_id, status)
    //                 VALUES(%d, %d, %d, %d, %d)";
    //                 db_query($sql, array($nid, $vid, $nid, $essay->essaytemplate_id, 2));
    //             }
    //         }
    //         //The section below is commented out to remove student information.
    //         //
    //         // drupal_set_message("Number of correct matriculation numbers in essays = $num_correct_matric");
    //         // drupal_set_message("Number of incorrect matriculation numbers in essays = $num_incorrect_matric");
    //     }	
    // }
    //initialise <instance> in the xml
    $instances = $imports->instance;
    $num_instances = sizeof($instances);

    drupal_set_message("Number of essays detected = " . $num_instances);

    for ($l=0;$l<$num_instances;$l++) {

        $instance = $instances[$l];
        
        if (isset($instance['nid'])) {
            $nid = $instance['nid'];
        } else {
            $nid = 0;
        }

        //The section below is added to reflect the changes in essay title. Stored in {node} & {node_revisions}, instead of {wamp_essay_template}
        if ($use_title_in_xml_file) $title = $instance['title'];

        //step 5 create and save essay
        $essay_contents = $instance->context;
        print_r($instance->context);
        $essay = new stdClass();
        $essay->is_new = TRUE;
        
        //The section below is commented out to remove student information.
        //Save the uid of the admin instead of the student.
        // $essay->uid = $author->uid;
        $essay->uid = $user->uid;

        $essay->created = time();
        $essay->type = 'wamp_essay';
        //The section below is commented out to remove essay template information.
        // $essay->essaytemplate_id = $essaytemplate_id;
        $essay->body = $essay_contents;
        $essay->format = 1;
        $essay->title = $title;
        $essay->op = t('Submit to Tutor');
        //The section below is commented out to remove student information.
        // $essay->name = $author->name;
        $essay->status = 1;
        $essay->comment = 2;
        $essay->validated = 1;
        $essay->revision = 1;
        $essay->log = t('Created New Draft');
        drupal_set_message("Essay NID = " . $nid);
        drupal_set_message('Essay Title = ' . $title);
        if ($essay->is_new && $test_run != 1) {
            
            //The section below is commented out to remove student information.
            //Save the uid of the admin instead of the student.
            // _node_save_revision($essay, $author->uid);
            _node_save_revision($essay, $user->uid);

            print_r($node);
            drupal_write_record('node', $essay);
            $vid = $nid;
            if ($nid == 0) { // chrhad
                $nid = $essay->nid; // follow natural order
                $vid = $essay->vid; // follow natural order
            }
            db_query('UPDATE {node} SET nid = %d, vid = %d WHERE nid = %d', $nid, $vid, $essay->nid);
            db_query('UPDATE {node_revisions} SET nid = %d, vid = %d WHERE vid = %d', $nid, $vid, $essay->vid);
            $op = 'insert';

            //The section below is commented out to remove student information.
            //
            // $sql = "INSERT INTO {wamp_essay_revision} (nid,vid,essay_id,essaytemplate_id,student_id, status)
            // VALUES(%d, %d, %d, %d, %d, %d)";
            // db_query($sql, array($nid, $vid, $nid, $essay->essaytemplate_id, $essay->uid, 2));
            //The section below is commented out to remove essay template information.
            //
            // $sql = "INSERT INTO {wamp_essay_revision} (nid, vid,essay_id, essaytemplate_id, status)
            // VALUES(%d, %d, %d, %d, %d)";
            // db_query($sql, array($nid, $vid, $nid, $essay->essaytemplate_id, 2));
            $sql = "INSERT INTO {wamp_essay_revision} (nid, vid,essay_id, status)
            VALUES(%d, %d, %d, %d)";
            db_query($sql, array($nid, $vid, $nid, 2));
        }
    }
}

/**
 * Build a form for exporting xml files
 *
 * Loads all modules and provides button to export xml.
 *
 * @ingroup forms
 */
function wamp_research_export_form() {

    //The section below is commented out to remove module information.
    //
    // $modules = array();
    // $modules["-1"] = "--- Please select a module ---";
    // $sql = "SELECT module_id, module_code, module_title
    //         FROM {wamp_module} WHERE display_flag=1";
    // $result = db_query($sql);
    // while ($module = db_fetch_object($result)) {
    //     $modules["$module->module_id"] = $module->module_code;
    // }

    // $form['module_id'] = array(
    //     '#type' => 'select',
    //     '#title' => t('Module'),
    //     '#options' => $modules,
    // );

    $form['buttons']['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Export',
    );
    // $form['buttons']['cancel'] = array(
    //     '#type' => 'markup',
    //     '#value' => '<input type="button" onclick="javascript:location.href=\'history.back();\'" value="Cancel">',
    // );
    $form['cancel'] = array(
        '#type' => 'markup',
        '#value' => t('<input type="button" id="research_form_cancel_btn" value="Cancel">'),
    );
    drupal_add_js(drupal_get_path('module', 'wamp_research').'/js/'.'research_form_cancel_event_handler.js');

    return $form;
}

function wamp_research_export_form_validate($form, $form_values) {

}

/**
 * Submit handler for postprocessing imported xml files. The export is performed
 * on a modular basis, i.e. to say, each export will contain all the assignment
 * prompts in the selected module, together with every student's submission and
 * their tutor's annotation. 
 *
 * Some Notes
 * - instance refers to a submission.
 * - context refers to the submitted essay in response to the assignment prompt.
 * - each instance can have more than 1 annotation
 *
 * The xml has this structure
 * <wamp>
 *    <module code="XXX">
 *       <group>
 *          <assignment title="XX">
 *             <instance matric="XX">
 *                <context>
 *                </context>
 *                <annotation annotator="XX">
 *                   <mistake start="//" end="//">
 *                      <type>XX</type>
 *                      <correction>XX</correction>
 *                      <comments>XX</comments>
 *                   </mistake>
 *                </annotation>
 *             </instance>
 *          </assignment>
 *       </group>
 *    </module>
 * </wamp>
 * 
 * @ingroup formapi
 * @see wamp_research_export_form()
 */
function wamp_research_export_form_submit($form, &$form_state) {

//variable init
    global $user;
    $record = $form_state['values'];

    //initialise xml parser
    $export = new SimpleXMLElement('<?xml version="1.0"?><wamp></wamp>');

    //add instance element
    $sql_instance = "
        SELECT r.nid, r.vid, nr.title, nr.body
        FROM {wamp_essay_revision} r, {node_revisions} nr
        WHERE r.nid = nr.nid
            AND r.status = 2";
    $result_instance = db_query($sql_instance);
    while ($instance_obj = db_fetch_object($result_instance)) {
        $instance = $export->addChild('instance');
        $instance->addAttribute('title', $instance_obj->title);

        //add context element
        $context = $instance->addChild('context',
            htmlentities($instance_obj->body));

        //add annotation element
        $sql_annotation = "SELECT annotator_id, annotations
                            FROM {wamp_essay_annotation}
                            WHERE nid=%d AND vid=%d AND annotator_id <> 1";
        $result_annotation = db_query($sql_annotation, 
            array($instance_obj->nid, $instance_obj->vid));
        while ($annotation_obj = db_fetch_object($result_annotation)) {

            //loading annotation xml per tutor
            $annotation_xml_from_db = $annotation_obj->annotations;
            $annotation_xml_from_db = '<?xml version="1.0"?>'.$annotation_xml_from_db;
            $annotation_simplexmlobj = simplexml_load_string($annotation_xml_from_db);

            $annotation = $instance->addChild('annotation');
            $annotation->addAttribute('annotator',
                get_display_name($annotation_obj->annotator_id));

            $mistakes = $annotation_simplexmlobj->mistake;
            $num_mistakes = sizeof($mistakes);
            for ($i=0;$i<$num_mistakes;$i++) {

                //adding mistake element
                $mistake_simplexmlobj = $mistakes[$i];
                $mistake = $annotation->addChild('mistake');
                $mistake->addAttribute('start', $mistake_simplexmlobj['start']);
                $mistake->addAttribute('end', $mistake_simplexmlobj['end']);

                //adding type element
                $type = $mistake->addChild('type', $mistake_simplexmlobj->type);

                //adding correction element
                $correction = $mistake->addChild('correction', $mistake_simplexmlobj->correction);

                //adding comments element
                $comments = $mistake->addChild('comments', $mistake_simplexmlobj->comments);
            }
        }
    }

    //writing the exported xml to a file
    $time = time();
    //The section below is commented out to remove module information.
    //
    // $dest = getcwd(). '/files/export/essay.'.$time . '.' . $module_obj->module_code . '.export.xml';
    // $dest_relative = 'files/export/essay.'.$time . '.' . $module_obj->module_code . '.export.xml';
    $dest = getcwd(). '/files/export/essay.'.$time . '.export.xml';
    $dest_relative = 'files/export/essay.'.$time . '.export.xml';

    //file_put_contents($dest, xmlpp(html_entity_decode($export->asXml()),false));
    file_put_contents($dest, html_entity_decode($export->asXml()));

    if (file_exists($dest)) {

        //drupal output for manual saving when user next clicks on the export tab
        $prefix = '/'.strtok($_SERVER['REQUEST_URI'], "/") . '/';
        drupal_set_message('File exported successfully');
        drupal_set_message('Download <a href="'.$prefix.$dest_relative.'" target=_blank>Exported XML file</a>');

        //saving the exports to database for later retrieval
        //The section below is commented out to remove module information.
        //
        // $sql_export = "INSERT INTO {wamp_essay_exported} (module_id, filename, exported_date, uid)
        //     VALUES(%d, '%s', NOW(), %d)";
        // db_query($sql_export, array($module_id, $dest_relative, $user->uid));
        $sql_export = "INSERT INTO {wamp_essay_exported} (filename, exported_date, uid)
            VALUES('%s', NOW(), %d)";
        db_query($sql_export, array($dest_relative, $user->uid));

        //sending the exported xml for immediate download
        global $HTTP_USER_AGENT;
        $file=basename($dest);
        $size = filesize($dest);
        header("Content-Type: application/octet-stream");
        header("Content-Type: application/force-download");
        header("Content-Length: $size");
        // IE5.5 just downloads index.php if we don't do this
        if(preg_match("/MSIE 5.5/", $HTTP_USER_AGENT)) {

            header("Content-Disposition: filename=$file");
        } else {

            header("Content-Disposition: attachment; filename=$file");
        }
        header("Content-Transfer-Encoding: binary");
        $fh = fopen($dest, "r");
        fpassthru($fh);
    }
}

/**
 * Build an overview form of xml exports.
 *
 * Loads all records and builds an overview form downloading exported xml
 *
 * @ingroup forms
 * @see theme_wamp_research_overview_form()
 * @see _wamp_research_overview_record_links()
 */
function wamp_research_export_overview_form() {

    $header = array(
        //The section below is commented out to remove module information.
        // array('data' => t('Module')),
        //The section below is commented out to change column name & field name
        // array('data' => t('Uploaded Date'), 'field' => 'uploaded_date'),
        array('data' => t('Exported Date'), 'field' => 'exported_date', 'sort' => 'desc'),
        array('data' => t('Exporter')),
        t('Operations')
    );

    //The section below is commented out to remove module information.
    // $sql = "select export_id, module_id, filename, exported_date, uid
    //             FROM {wamp_essay_exported} e";
    $sql = "select export_id, filename, exported_date, uid
                FROM {wamp_essay_exported} e";

    $sql .= tablesort_sql($header);
    $query_count = 'select count(export_id) FROM {wamp_essay_exported} e';
    $result = pager_query($sql, 50, 0, $query_count);
    $destination = drupal_get_destination();

    while ($export = db_fetch_object($result)) {
        //The section below is commented out to remove module information.
        //
        // $sql_modulecode = "SELECT m.module_code from {wamp_module} m where m.module_id = %d";
        // $modulecode = db_result(db_query($sql_modulecode, array($export->module_id)));
        
        // $form['module'][$export->export_id] = array('#value' => $modulecode);
        //$form['filename'][$export->export_id] = array('#value' => $export->filename);
        $form['exported_date'][$export->export_id] = array('#value' => $export->exported_date);

        $exporter = user_load(array('uid' => $export->uid));

        $form['uid'][$export->export_id] = array('#value' => $exporter->name);

        $form['operations'][$export->export_id] = array(
            '#type' => 'markup',
            '#value' => _wamp_research_export_overview_record_links($export),
        );
    }

    return $form;
}

/**
 * Build the edit and delete links for export overview form
 *
 * @see wamp_module_export_overview_form
 */
function _wamp_research_export_overview_record_links($record) {

    $links['download'] = array(
        'title' => t('Download'),
        'href' => $prefix.$record->filename,
        'html' => TRUE,
        'target' => '_blank',
    );

    return theme('links', $links);
}

/**
 * Theme wamp essay xml export overview form
 *
 * @ingroup themeable
 * @see wamp_research_export_overview_form()
 */
function theme_wamp_research_export_overview_form($form) {

    $header = array(
        //The section below is commented out to remove module information.
        // array('data' => t('Module')),
        array('data' => t('Exported Date')),
        array('data' => t('Exporter')),
        t('Operations')
    );


    if (isset($form['operations']) && is_array($form['operations'])) {
        foreach (element_children($form['operations']) as $key) {
        //       print_r($form['modulecode']);
            $rows[] = array(

                //The section below is commented out to remove module information.
                // drupal_render($form['module'][$key]),
                // drupal_render($form['filename'][$key]),
                drupal_render($form['exported_date'][$key]),
                drupal_render($form['uid'][$key]),
                drupal_render($form['operations'][$key]),
            );
        }
    }
    else {
        $rows[] = array(array('data' => t('No exported xml files available.'), 'colspan' => '5'));
    }

    $output .= theme('table', $header, $rows);
    if ($form['pager']['#value']) {
        $output .= drupal_render($form['pager']);
    }

    $output .= drupal_render($form);

    return $output;
}


/**
 * Build the delete imported xml confirmation form.
 *
 * A simple wrapper around Drupal's core confirm_form() function. Adds a value
 * field to store the ID of the record being deleted.
 *
 * @ingroup forms
 * @see wamp_research_delete_confirm_submit()
 * @see confirm_form()
 */
function wamp_research_delete_confirm(&$form_state, $record=array()) {

    $form['import_id'] = array(
        '#type' => 'value',
        '#value' => $record['import_id'],
    );

	$form['uploaded_filename'] = array(
		'#type' => 'value',
		'#value' => 'essay.'.strtotime($record['uploaded_date']).'.'.$record['original_filename'],
	);

    return confirm_form($form,
    t('Are you sure you want to delete %file uploaded on %date?',
		array('%file' => $record['original_filename'], '%date' => $record['uploaded_date'])),
    'admin/wamp/wamp_research/',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
    );
}

/**
 * General submit handler for the delete imported xml confirmation form.
 *
 * Core's confirm_form() function adds the 'confirm' value element we check
 * against to ensure the form was properly submitted. If it's there, delete
 * the record and redirect to the overview form.
 *
 * @ingroup formapi
 * @see wamp_research_delete_confirm()
 */

function wamp_research_delete_confirm_submit($form, &$form_state) {

    if ($form_state['values']['confirm']) {
        db_query('DELETE FROM {wamp_essay_imported} WHERE import_id=%d', $form_state['values']['import_id']);
		$filename = getcwd().'/files/'.$form_state['values']['uploaded_filename'];
		file_delete($filename);
        drupal_set_message(t('The imported xml file is removed.'));
    }
    $form_state['redirect'] = 'admin/wamp/wamp_research/';
}

/**
 * Build a form for exporting m2 files
 *
 * Loads all modules and provides button to export xml.
 *
 * @ingroup forms
 */
function wamp_research_export_m2_form() {

    $essays = array();
    $essays["-1"] = "--- Please select an essay ---";

    //The section below is commented out to remove student information.
    //
    // $sql_essay = "
    //     SELECT n.nid, n.vid, n.title, u.uid, u.name AS student_name
    //     FROM 
    //         {node} n
    //     JOIN 
    //         {users} u
    //     ON n.uid = u.uid
    //     WHERE n.status=1
    //         AND nid IN (SELECT nid FROM {wamp_essay_annotation})";
    $sql_essay = "
        SELECT r.nid, r.vid, nr.title, nr.body
        FROM {wamp_essay_revision} r, {node_revisions} nr
        WHERE r.nid = nr.nid
            AND r.status = 2
            AND r.nid IN (SELECT nid FROM {wamp_essay_annotation})";

    $result_essay = db_query($sql_essay);
    while ($essay = db_fetch_object($result_essay)) {
        //The section below is commented out to remove student information.
        // $essays["$essay->nid"] = ($essay->title . ' [' . $essay->student_name . ']');
        $essays["$essay->nid"] = ($essay->title);
    }

    $annotators = array();
    $annotators["-1"] = "--- Please select an annotator ---";
    $sql_annotator = "
        SELECT uid, value AS name
        FROM {profile_values}
        WHERE uid IN (SELECT annotator_id FROM {wamp_essay_annotation})
            AND fid = 1";
    $result_annotator = db_query($sql_annotator);
    while ($annotator = db_fetch_object($result_annotator)) {
        $annotators["$annotator->uid"] = $annotator->name;
    }

    $form['nid'] = array(
        '#type' => 'select',
        '#title' => t('Essay'),
        '#options' => $essays,
    );

    $form['uid'] = array(
        '#type' => 'select',
        '#title' => t('Annotator'),
        '#options' => $annotators,
    );

    $form['buttons']['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Export',
    );
    // $form['buttons']['cancel'] = array(
    //     '#type' => 'markup',
    //     '#value' => '<input type="button" onclick="javascript:location.href=\'history.back();\'" value="Cancel">',
    // );
    $form['cancel'] = array(
        '#type' => 'markup',
        '#value' => t('<input type="button" id="research_form_cancel_btn" value="Cancel">'),
    );
    drupal_add_js(drupal_get_path('module', 'wamp_research').'/js/'.'research_form_cancel_event_handler.js');

    return $form;
}

/**
 * Validate if the selected annotator has annotated the selected essay.
 * If not, return error and prompt the user to select another annotator.
 */
function wamp_research_export_m2_form_validate($form, $form_values) {
    $nid = (int) $form_values['values']['nid'];
    $uid = (int) $form_values['values']['uid'];

    if (empty($nid) || $nid == -1) {
        form_set_error('nid', t('You have not selected any essay. Please select an essay from the dropdown list.'));
    };

    if (empty($uid) || $uid == -1) {
        form_set_error('uid', t('You have not selected any annotator. Please select an annotator from the dropdown list.'));
    };

    //fetch annotation data
    $sql_annotation =
        "SELECT nid, vid, annotator_id, annotations
        FROM {wamp_essay_annotation}
        WHERE nid=%d and annotator_id=%d";
    $result_annotation = db_query($sql_annotation, array($nid, $uid));
    $annotation_obj = db_fetch_object($result_annotation);
    if (!$annotation_obj) {
        form_set_error('uid', t('The selected annotator has not annotated the essay yet. Please select another annotator.'));
    };
}

/**
 * Submit handler for postprocessing annotator's annotation. The export is performed
 * on a annotation basis, i.e. to say, each export will contain all the corrected
 * mistakes in the selected annotator's annotation. 
 *
 * Some Notes
 * - P refers to a paragraph, enclosing the sentences of the submitted essay
 * - start_par counts from 1, while start_off counts from 0
 * - each ANNOTATION can have more than 1 MISTAKE
 * - COMMENT is optional
 *
 * The xml has this structure
 * <DOC nid="//">
 *     <TEXT>
 *         <TITLE>XX</TITLE>
 *         <P>
 *         </P>
 *         ...
 *         <P>
 *         </P>
 *     </TEXT>
 *     <ANNOTATION annotator_id="//">
 *         <MISTAKE start_par="//" start_off="//" end_par="//" end_off="//">
 *             <TYPE>XX</TYPE>
 *             <CORRECTION>XX</CORRECTION>
 *             <COMMENT>XX</COMMENT>
 *         </MISTAKE>
 *     </ANNOTATION>
 * </DOC>
 * 
 * @ingroup formapi
 * @see wamp_research_export_m2_form()
 */
function wamp_research_export_m2_form_submit($form, &$form_state) {

//variable init
    global $user;
    $record = $form_state['values'];
    $nid = (int) $record['nid'];
    $uid = (int) $record['uid'];

    if (!empty($nid) && $nid > -1 && !empty($uid) && $uid > -1) {
        
        //initialise xml parser
        $export = new SimpleXMLElement('<DOC nid="' . $nid . '"></DOC>');

        //fetch essay data
        $sql_essay =
            "SELECT nid, vid, uid, title, body
            FROM {node_revisions}
            WHERE nid=%d";
        $result_essay = db_query($sql_essay, array($nid));
        $essay_obj = db_fetch_object($result_essay);
        
        //add text element
        $text = $export->addChild("TEXT");
        
        //add title element
        $title = $text->addChild("TITLE", $essay_obj->title);

        //add body element
        //paragraph segmentation, based on delimiter <br/><br/>
        $body_array = preg_split ('[<br/><br/>]', $essay_obj->body);
        foreach ($body_array as $paragraph) {
            $text->addChild("P", $paragraph);
        }

        //fetch annotation data
        $sql_annotation =
            "SELECT nid, vid, annotator_id, annotations
            FROM {wamp_essay_annotation}
            WHERE nid=%d and annotator_id=%d";
        $result_annotation = db_query($sql_annotation, array($nid, $uid));
        $annotation_obj = db_fetch_object($result_annotation);

        //add annotation element
        $annotation = $export->addChild("ANNOTATION");
        $annotation->addAttribute('annotator_id', $annotation_obj->annotator_id);

        //fetch mistake data
        //sort mistakes in descending order of the start position
        $annotation_simplexmlobj = simplexml_load_string($annotation_obj->annotations);
        function sort_mistakes($m1, $m2) {
            $m1_start = explode(".", end(explode("/", $m1['start'])));
            $m2_start = explode(".", end(explode("/", $m2['start'])));
            $m1_start_par = (int) $m1_start[0];
            $m1_start_off = (int) (($m1_start) > 1 ? (int) $m1_start[1] : 0);
            $m2_start_par = (int) $m2_start[0];
            $m2_start_off = (int) (sizeof($m2_start) > 1 ? (int) $m2_start[1] : 0);
            if ($m1_start_par == $m2_start_par) {
                if ($m1_start_off == $m2_start_off) {
                    return 0;
                } elseif ($m1_start_off > $m2_start_off) {
                    return 1;
                } else {
                    return -1;
                }
            } elseif ($m1_start_par > $m2_start_par) {
                return 1;
            } else {
                return -1;
            }
            return $result;
        }
        foreach($annotation_simplexmlobj->mistake as $mistake) {
            $mistakes[] = $mistake;
        }
        usort($mistakes, 'sort_mistakes');

        // $mistakes = $annotation_simplexmlobj->mistake;
        $num_mistakes = sizeof($mistakes);
        for ($i=0;$i<$num_mistakes;$i++) {

            //add mistake elements
            $mistake_simplexmlobj = $mistakes[$i];
            $mistake = $annotation->addChild('MISTAKE');
            
            //parse start attributes
            $mistake_start_substr = end(explode("/", $mistake_simplexmlobj['start']));
            $mistake_start_array = explode(".", $mistake_start_substr);
            $mistake->addAttribute('start_par', (int) ceil(($mistake_start_array[0] + 1) / 3));
            if (sizeof($mistake_start_array) < 2) {
                $mistake->addAttribute('start_off', 0);
            } else {
                $mistake->addAttribute('start_off', $mistake_start_array[1]);
            }
            //parse end attributes
            $mistake_end_substr = end(explode("/", $mistake_simplexmlobj['end']));
            $mistake_end_array = explode(".", $mistake_end_substr);
            $mistake->addAttribute('end_par', (int) ceil(($mistake_end_array[0] + 1) / 3));
            if (sizeof($mistake_end_array) < 2) {
                $mistake->addAttribute('end_off', 0);
            } else {
                $mistake->addAttribute('end_off', $mistake_end_array[1]);
            }

            //add type element
            $type = $mistake->addChild('TYPE', $mistake_simplexmlobj->type);

            //add correction element
            $correction = $mistake->addChild('CORRECTION', $mistake_simplexmlobj->correction);

            //add comment element
            $comment = $mistake->addChild('COMMENT', $mistake_simplexmlobj->comments);
        }
        
        //writing the exported xml to a file
        $dest_xml = getcwd(). '/files/export/m2/export_for_m2.xml';
        $dest_relative_xml = 'files/export/m2/export_for_m2.xml';
        // file_put_contents($dest, xmlpp(html_entity_decode($export->asXml()),false));
		file_put_contents($dest_xml, html_entity_decode($export->asXml()));

        $dest_script = getcwd(). '/nucle/scripts/preprocess.py';
        $dest_export = getcwd(). '/files/export/m2';
        $dest_export_conll = $dest_export . '/output.conll';
        $dest_export_ann = $dest_export . '/output.conll.ann';
        $dest_export_m2 = $dest_export . '/output.conll.m2';
        $dest_relative_m2 = 'files/export/m2/output.conll.m2';
        //use Miniconda2 to set up Python2 enviornment to run preprocess.py
        shell_exec('/conda/bin/python ' . $dest_script . ' -l ' . $dest_xml . ' ' . $dest_export_conll . ' ' . $dest_export_ann . ' ' . $dest_export_m2);

        if (file_exists($dest_export_m2)) {

            //drupal output for manual saving when user next clicks on the export tab
            $prefix = '/'.strtok($_SERVER['REQUEST_URI'], "/") . '/';
            drupal_set_message('File exported successfully');
            drupal_set_message('Download <a href="'.$prefix.$dest_relative_m2.'" target=_blank>Exported XML file</a>');

            //sending the exported xml for immediate download
            global $HTTP_USER_AGENT;
            //generate a new filename, based on nid(essay) and uid(annotator)
            // $file=basename($dest_export_m2);
            $essay_title = format_filename($essay_obj->title);
            //The seciton below is commented out to remove student information.
            // $student_matric_num = str_replace(' ', '_', get_student_matric_num($essay_obj->uid));
            $annotator_name = format_filename(get_annotator_name($uid)); 
            //The seciton below is commented out to remove student information.
            // $file = time() . '.' . $essay_title . '.' . $student_matric_num . '.' . $annotator_name .'.m2';
            $file = time() . '.' . $essay_title . '.' . $annotator_name .'.m2';
            $size = filesize($dest_export_m2);
            header("Content-Type: application/octet-stream");
            header("Content-Type: application/force-download");
            header("Content-Length: $size");
            // IE5.5 just downloads index.php if we don't do this
            if(preg_match("/MSIE 5.5/", $HTTP_USER_AGENT)) {

                header("Content-Disposition: filename=$file");
            } else {

                header("Content-Disposition: attachment; filename=$file");
            }
            header("Content-Transfer-Encoding: binary");
            $fh = fopen($dest_export_m2, "r");
            fpassthru($fh);
        }

    }

}

/**
 * Build a form for configuring error types
 *
 * Supported error types are as following:
 * 1) CoNLL-2014 (default)
 * 2) ERRANT
 *
 * @ingroup forms
 */
function wamp_research_error_type_form() {

    $error_types = array('CoNLL-2014', 'ERRANT');

    $form['error_type'] = array(
        '#type' => 'select',
        '#title' => t('Error Type'),
        '#options' => $error_types,
        '#default_value' => variable_get('error_type', 0),
    );

    $form['buttons']['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Submit',
    );
    // $form['buttons']['cancel'] = array(
    //     '#type' => 'markup',
    //     '#value' => '<input type="button" onclick="javascript:location.href=\'history.back();\'" value="Cancel">',
    // );
    $form['cancel'] = array(
        '#type' => 'markup',
        '#value' => t('<input type="button" id="research_form_cancel_btn" value="Cancel">'),
    );
    drupal_add_js(drupal_get_path('module', 'wamp_research').'/js/'.'research_form_cancel_event_handler.js');

    return $form;
}

/**
 * Submit handler for configuring error type.
 * 
 * 
 * @ingroup formapi
 * @see wamp_research_error_type_form()
 */
function wamp_research_error_type_form_submit($form, &$form_state) {
//variable init
    $record = $form_state['values'];
    $error_type = $record['error_type'];
    variable_set("error_type", $error_type);
    if ($error_type == variable_get("error_type", -1)) {
        drupal_set_message('Error type configured successfully');
    }
}

function format_filename($orig_file_name) {
    //replace all whitespaces with underscores
    $new_file_name = str_replace(' ', '_', $orig_file_name);
    //remove all special characters
    $new_file_name = preg_replace('/[^A-Za-z0-9\-\_]/', '', $new_file_name);
    return $new_file_name;
}

/**
 * Convert curly/smart quotes to regular quotes
 *
 * This function will convert Windows-1252, CP-1252, and other UTF-8 single and double quotes to regular quotes,
 * otherwise known as Unicode character U+0022 quotion mark (") and U+0027 apostrophe (') which typically do not have
 * any sort of encoding issues that the others run into.
 *
 * @author https://www.ozzu.com/snippets/608141/convert-curly-quotes-to-regular-quotes-in-php
 * @param string $text The text that contains curly quotes
 * @return string Normalized text using regular quotes
 */
function convert_curly_quotes($text) {
    $quote_mapping = array(
        // U+0082⇒U+201A single low-9 quotation mark
        "\xC2\x82"     => "'",

        // U+0084⇒U+201E double low-9 quotation mark
        "\xC2\x84"     => '"',

        // U+008B⇒U+2039 single left-pointing angle quotation mark
        "\xC2\x8B"     => "'",

        // U+0091⇒U+2018 left single quotation mark
        "\xC2\x91"     => "'",

        // U+0092⇒U+2019 right single quotation mark
        "\xC2\x92"     => "'",

        // U+0093⇒U+201C left double quotation mark
        "\xC2\x93"     => '"',

        // U+0094⇒U+201D right double quotation mark
        "\xC2\x94"     => '"',

        // U+009B⇒U+203A single right-pointing angle quotation mark
        "\xC2\x9B"     => "'",

        // U+00AB left-pointing double angle quotation mark
        "\xC2\xAB"     => '"',

        // U+00BB right-pointing double angle quotation mark
        "\xC2\xBB"     => '"',

        // U+2018 left single quotation mark
        "\xE2\x80\x98" => "'",

        // U+2019 right single quotation mark
        "\xE2\x80\x99" => "'",

        // U+201A single low-9 quotation mark
        "\xE2\x80\x9A" => "'",

        // U+201B single high-reversed-9 quotation mark
        "\xE2\x80\x9B" => "'",

        // U+201C left double quotation mark
        "\xE2\x80\x9C" => '"',

        // U+201D right double quotation mark
        "\xE2\x80\x9D" => '"',

        // U+201E double low-9 quotation mark
        "\xE2\x80\x9E" => '"',

        // U+201F double high-reversed-9 quotation mark
        "\xE2\x80\x9F" => '"',

        // U+2039 single left-pointing angle quotation mark
        "\xE2\x80\xB9" => "'",

        // U+203A single right-pointing angle quotation mark
        "\xE2\x80\xBA" => "'",

        // HTML left double quote
        "&ldquo;"      => '"',

        // HTML right double quote
        "&rdquo;"      => '"',

        // HTML left sinqle quote
        "&lsquo;"      => "'",

        // HTML right single quote
        "&rsquo;"      => "'",
    );

    return strtr(html_entity_decode($text, ENT_QUOTES, "UTF-8"), $quote_mapping);
}

/**
 * @} End of "ingroup wamp_research".
 */
?>
