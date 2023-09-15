<?php
// @author Chan Hong Guan chanhg -at- comp.nus.edu.sg
// $Id: wamp_essay.admin.inc 164 2009-08-17 11:08:32Z chanhg $


/**
 * @ingroup wamp_essay
 * @{
 */

//The section below is commented out to remove essay template information.
//
// /**
//  * Build an overview form of essay template records.
//  *
//  * Loads all records and builds an overview form with links for adding/listing
//  * of essay templates showing the corresponding module.
//  *
//  * @ingroup forms
//  * @see theme_wamp_essay_overview_form()
//  * @see _wamp_essay_template_record_links()
//  */
// function wamp_essay_template_overview_form() {
// 	$header = array(
// 		array('data' => t('Essay Title'), 'field' => 'e.title'),
// 		//The section below is commented out to remove module information.
// 		//
// 		// array('data' => t('Module'), 'field' => 'm.module_code'),
// 		// array('data' => t('Creator')),
// 		t('Operations')
// 	);

// 	//display all essay templates that have display flag switched on
// 	//The section below is commented out to remove module information.
// 	//
// 	// $sql = "select e.essaytemplate_id, e.title, e.module_id, m.module_code, e.annotator_id
//     //             FROM {wamp_essay_template} e, {wamp_module} m
//     //             WHERE m.module_id = e.module_id AND e.display_flag = 1
// 	// 			AND m.display_flag=1";
// 	$sql = "select e.essaytemplate_id, e.title, e.annotator_id
//                 FROM {wamp_essay_template} e
//                 WHERE e.display_flag = 1";

// 	$sql .= tablesort_sql($header);
// 	$query_count = 'select COUNT(DISTINCT e.essaytemplate_id) from {wamp_essay_template} e';
// 	//    $result = db_query($sql);
// 	$result = pager_query($sql, 50, 0, $query_count);
// 	$destination = drupal_get_destination();


// 	$essays = array();
// 	while ($essay = db_fetch_object($result)) {

// 		//The section below is commented out to remove module information.
// 		// $modules[$essay->essaytemplate_id] = '';
// 		$form['title'][$essay->essaytemplate_id] = array('#value' => $essay->title);

// 		//The section below is commented out to remove module information.
// 		//
// 		// $form['creator'][$essay->essaytemplate_id] = array('#value' => get_display_name($essay->annotator_id));
// 		// $form['module'][$essay->essaytemplate_id] = array('#value' => $essay->module_code);

// 		$form['operations'][$essay->essaytemplate_id] = array(
// 			'#type' => 'markup',
// 			'#value' => _wamp_essay_template_record_links($essay),
// 		);
// 	}



// 	return $form;
// }

//The section below is commented out to remove essay template information.
//
// /**
//  * Build the edit and delete links for an essay template.
//  *
//  * @see wamp_essay_template_overview_form()
//  */
// function _wamp_essay_template_record_links($record) {
// 	$links['edit'] = array(
// 		'title' => t('Update'),
// 		'href' => "admin/wamp/wamp_essay/".$record->essaytemplate_id."/edit",
// 		'html' => TRUE,
// 	);

// 	$links['delete'] = array(
// 		'title' => t('Delete'),
// 		'href' => "admin/wamp/wamp_essay/".$record->essaytemplate_id."/delete",
// 		'html' => TRUE,
// 	);

// 	$links['show_essays'] = array(
// 		'title' => t('Show Essay Submissions'),
// 		'href' => "admin/wamp/wamp_essay/".$record->essaytemplate_id."/show_essay_submissions",
// 		'html' => TRUE,
// 	);

// 	return theme('links', $links);
// }

//The section below is commented out to remove essay template information.
//
// /**
//  * Theme wamp essay template overview form
//  *
//  * @ingroup themeable
//  */
// function theme_wamp_essay_template_overview_form($form) {
// // Overview table:
// 	$header = array(
// 		array('data' => t('Essay Title'), 'field' => 'e.title'),
// 		//The section below is commented out to remove module information.
// 		//
// 		// array('data' => t('Module'), 'field' => 'm.module_code'),
// 		// array('data' => t('Creator')),
// 		t('Operations')
// 	);


// 	if (isset($form['title']) && is_array($form['title'])) {
// 		foreach (element_children($form['title']) as $key) {
// 			$rows[] = array(

// 				drupal_render($form['title'][$key]),
// 				//The section below is commented out to remove module information.
// 				//
// 				// drupal_render($form['module'][$key]),
// 				// drupal_render($form['creator'][$key]),
// 				drupal_render($form['operations'][$key]),
// 			);
// 		}
// 	}
// 	else {
// 		$rows[] = array(array('data' => t('No essay templates available.'), 'colspan' => '4'));
// 	}

// 	$output .= theme('table', $header, $rows);
// 	if ($form['pager']['#value']) {
// 		$output .= drupal_render($form['pager']);
// 	}

// 	$output .= drupal_render($form);

// 	return $output;
// }

//The section below is commented out to remove essay template information.
//
// /**
//  * Build the essay template record creating/editing form.
//  *
//  * If a record is passed in, an edit form with both Update, Delete and Cancel buttons
//  * will be built. Otherwise, a blank 'Add Essay' form, with the Create Module
//  * and Cancel buttons, will be built.
//  *
//  * @ingroup forms
//  * @see wamp_essay_template_form_submit()
//  * @see wamp_essay_template_form_validate()
//  */
// function wamp_essay_template_form(&$form_state, $record = array()) {
// // Set the default values for a new item. By using += rather than =, we
// // only overwrite array keys that have not yet been set. It's safe to use
// // on both an empty array, and an incoming array with full or partial data.
// //$record += array(
// //'module_title' => '',
// //);

// //bring the $user into context
// 	global $user;

// 	// If we're editing an existing record, we'll add a value field to the form
// 	// containing the record's unique ID.
// 	if (!empty($record['essaytemplate_id'])) {
// 		$form['essaytemplate_id'] = array(
// 			'#type' => 'value',
// 			'#value' => $record['essaytemplate_id'],
// 		);
// 	}

// 	//The section below is commented out to remove module information.
// 	//
// 	// if ($user->uid == 1) {
// 	// 	$sql_modules = "SELECT m.module_id, m.module_code, m.module_title from {wamp_module} m";
// 	// 	$result_modules = db_query($sql_modules);
// 	// 	$modules = array();
// 	// 	$modules["-1"] = "--- Please select a module ---";
// 	// 	while ($module = db_fetch_object($result_modules)) {

// 	// 		$modules["$module->module_id"]  = t($module->module_code . " " . $module->module_title);
// 	// 	}
// 	// }

// 	$form['title'] = array(
// 		'#type' => 'textfield',
// 		'#title' => t('Essay Title'),
// 		'#default_value' => $record['title'],
// 		'#required' => TRUE,
// 	);

// 	$form['description'] = array(
// 		'#type' => 'textarea',
// 		'#title' => t('Description'),
// 		'#required' => TRUE,
// 		'#default_value' => $record['description'],
// 	);

// 	//The section below is commented out to remove module information.
// 	//
// 	// $form['module_id'] = array(
// 	// 	'#type' => 'select',
// 	// 	'#options' => $modules,
// 	// 	'#title' => t('Module'),
// 	// 	'#default_value' => $record['module_id']
// 	// );

// 	if (!empty($record['essaytemplate_id'])) {
// 		$form['buttons']['update'] = array(
// 			'#type' => 'submit',
// 			'#value' => t('Update'),
// 		);
// 	} else {
// 		$form['buttons']['create'] = array(
// 			'#type' => 'submit',
// 			'#value' => t('Create Essay Template'),
// 		);
// 	}


// 	// Only show the delete button if we already have an ID. Set the delete
// 	// button's submit handler to a custom function that should only fire if
// 	// this button is clicked. In all other cases, the form will fall back to
// 	// the default $form_id_submit() function.
// 	if (!empty($record['essaytemplate_id'])) {
// 		$form['delete'] = array(
// 			'#type' => 'submit',
// 			'#value' => t('Delete'),
// 			'#submit' => array('wamp_essay_template_custom_delete_submit'),
// 		);
// 	}

// 	return $form;
// }

//The section below is commented out to remove essay template information.
//
// /**
//  * Validation routine for adding and updating essay template records
//  * Ensures that an essay template is assigned to the module
//  * 
//  * @see wamp_essay_template_form
//  */
// function wamp_essay_template_form_validate($form, $form_values) {

// 	//The section below is commented out to remove module information.
// 	// if ($form_values['values']['module_id'] == '-1')

// 	// 	form_set_error('module_id', t('Please select a module.'));
// }

//The section below is commented out to remove essay template information.
//
// /**
//  * General submit handler for essay template's add/edit form.
//  *
//  * Simply passes incoming form values on to the module's CRUD save function,
//  * then redirects to the overview form.
//  *
//  * @ingroup formapi
//  * @see wamp_essay_template_form()
//  */
// function wamp_essay_template_form_submit($form, &$form_state) {

// 	global $user;

// 	$record = $form_state['values'];
// 	$record['annotator_id'] = $user->uid;
// 	$record['display_flag'] = 1;
// 	$my_record = wamp_essay_record_save($record);

// 	if ($record['essaytemplate_id'] != '')
// 		drupal_set_message(t('Essay updated successfully'));
// 	else drupal_set_message(t('Essay created successfully'));
// 	//$form_state['redirect'] = 'admin/wamp/wamp_essay';
// 	drupal_goto('admin/wamp/wamp_essay');
// }

//The section below is commented out to remove essay template information.
//
// function wamp_essay_template_custom_delete_submit($form, &$form_state) {

// 	$form_state['redirect'] = 'admin/wamp/wamp_essay/'.$form_state['values']['essaytemplate_id'].'/delete';
// }

//The section below is commented out to remove essay template information.
//
// /**
//  * Build the remove essay template confirmation form.
//  *
//  * A simple wrapper around Drupal's core confirm_form() function. Adds a value
//  * field to store the ID of the record being deleted.
//  *
//  * @ingroup forms
//  * @see confirm_form()
//  */
// function wamp_essay_template_delete_confirm(&$form_state, $record=array()) {

// 	$form['essaytemplate_id'] = array(
// 		'#type' => 'value',
// 		'#value' => $record['essaytemplate_id'],
// 	);

// 	return confirm_form($form,
// 	t('Are you sure you want to delete %title?', array('%title' => $record['title'])),
// 	isset($_GET['destination']) ? $_GET['destination'] : 'admin/wamp/wamp_essay',
// 	t('This action cannot be undone.'),
// 	t('Delete'),
// 	t('Cancel')
// 	);
// }

//The section below is commented out to remove essay template information.
//
// /**
//  * General submit handler for the remove essay template confirmation form.
//  *
//  * Core's confirm_form() function adds the 'confirm' value element we check
//  * against to ensure the form was properly submitted. If it's there, delete
//  * the record and redirect to the overview form.
//  *
//  * @ingroup forms
//  * @see wamp_essay_template_delete_confirm()
//  */

// function wamp_essay_template_delete_confirm_submit($form, &$form_state) {

// //  print_r($form_state['values']);
// 	if ($form_state['values']['confirm']) {
// 		wamp_essay_record_delete($form_state['values']['essaytemplate_id']);
// 		drupal_set_message(t('The essay was deleted.'));
// 	}
// 	$form_state['redirect'] = 'admin/wamp/wamp_essay';
// }

/**
 * Build a form to display the essays submitted for an essay template
 *
 * Loads all records and builds an overview form with links for editing,
 * removing access to the essay. There are also links to view the annotated
 * essay (if there are annotations)
 *
 * @ingroup forms
 * @see theme_wamp_essay_show_essay_submissions_form()
 * @see _wamp_essay_show_essay_submissions_links()
 */
function wamp_essay_show_essay_submissions_form(&$form_state, $record = array()) {
	$header = array(
		array('data' => t('No.')),
		array('data' => t('Essay ID'), 'field' => 'r.essay_id'),

		//The section below is commented out to remove student information.
		//
		// array('data' => t('Student Name')),
		// array('data' => t('Matric #'), 'field' => 'pv.value'),
		//The section below is commented out to remove module information.
		// array('data' => t('Group'), 'field' => 'mg.group_name'),
		
		//The section below is added to display the essay title.
		array('data' => t('Essay Title'), 'class' => 'center', 'field' => 'nr.title'),

		array('data' => t('Bad Essay'), 'class' => 'center', 'field' => 'r.bad_essay'),
		array('data' => t('Needs Editing'), 'class' => 'center', 'field' => 'r.needs_editing'),
		array('data' => t('Status')),
		t('Operations')
	);

	//The section below is commented out to remove module information.
	// $sql = "SELECT r.essay_id, r.student_id, r.nid, r.vid, mg.group_name, r.status, r.bad_essay, r.needs_editing, mg.modulegroup_id
	// 		FROM {wamp_essay_revision} r, {wamp_module} m, {wamp_essay_template} t,
	// 			{wamp_module_group} mg, {wamp_module_group_student} mgs,
	// 			{profile_fields} pf, {profile_values} pv
    //         WHERE m.module_id = t.module_id AND r.essaytemplate_id = t.essaytemplate_id
    //             AND r.essaytemplate_id = " . $record['essaytemplate_id']."
	// 			AND m.module_id = mg.module_id
	// 			AND mg.modulegroup_id = mgs.modulegroup_id
	// 			AND mgs.student_id = r.student_id
	// 			AND pf.fid = pv.fid AND pf.name='profile_pers_matricnum' AND pv.uid = r.student_id";
	//The section below is commented out to remove student information.
	//
	// $sql = "SELECT r.essay_id, r.student_id, r.nid, r.vid, r.status, r.bad_essay, r.needs_editing
	// 		FROM {wamp_essay_revision} r, {wamp_essay_template} t,
	// 			{profile_fields} pf, {profile_values} pv
    //         WHERE r.essaytemplate_id = t.essaytemplate_id
    //             AND r.essaytemplate_id = " . $record['essaytemplate_id']."
	// 			AND pf.fid = pv.fid AND pf.name='profile_pers_matricnum' AND pv.uid = r.student_id";
	//The section below is commented out to remove essay template information.
	//
	// $sql = "SELECT r.essay_id, r.nid, r.vid, r.status, r.bad_essay, r.needs_editing
	// 		FROM {wamp_essay_revision} r, {wamp_essay_template} t
    //         WHERE r.essaytemplate_id = t.essaytemplate_id
    //             AND r.essaytemplate_id = " . $record['essaytemplate_id'];

	//The section below is commented out to reflect the addition of essay title.
	//
	// $sql = "SELECT r.essay_id, r.nid, r.vid, r.status, r.bad_essay, r.needs_editing
	// 		FROM {wamp_essay_revision} r";
	$sql = 'SELECT r.essay_id, r.nid, r.vid, r.status, r.bad_essay, r.needs_editing, nr.title, nr.body
	FROM {wamp_essay_revision} r, {node_revisions} nr
	WHERE r.nid = nr.nid';

	$sql .= tablesort_sql($header);
	//The section below is commented out to remove module information.
	//
	// $query_count = "SELECT COUNT(r.nid)
    //         FROM {wamp_essay_revision} r, {wamp_module} m, {wamp_essay_template} t
    //         WHERE m.module_id = t.module_id AND r.essaytemplate_id = t.essaytemplate_id
    //             AND r.essaytemplate_id = " . $record['essaytemplate_id'];
	//The section below is commented out to remove essay template information.
	//
	// $query_count = "SELECT COUNT(r.nid)
    //         FROM {wamp_essay_revision} r, {wamp_essay_template} t
    //         WHERE r.essaytemplate_id = t.essaytemplate_id
    //             AND r.essaytemplate_id = " . $record['essaytemplate_id'];
	
	//The section below is commented out to reflect the addition of essay title.
	//
	// $query_count = "SELECT COUNT(r.nid)
    //         FROM {wamp_essay_revision} r";
	$query_count = 'SELECT COUNT(r.nid)
	FROM {wamp_essay_revision} r, {node_revisions} nr
	WHERE r.nid = nr.nid';

	//    $result = db_query($sql);
	$result = pager_query($sql, 50, 0, $query_count);
	//	$result = db_query($sql);
	$destination = drupal_get_destination();


	$essays = array();
	$i =0;
	while ($essayrevision = db_fetch_object($result)) {

	//        $essays[$essay->essaytemplate_id] = '';
	//        $form['student_id'][$essay->essaytemplate_id] = array('#value' => $essayrevision->student_id);
	//
		/**
		 * The section below is commented out to remove student information.
		 * 
		$sql_studentname = "SELECT f.name, v.value  FROM {users} u, {profile_fields} f, {profile_values} v
            WHERE u.uid = v.uid AND u.uid = %d
            AND f.fid = v.fid
            AND f.name = 'profile_pers_matricnum'";
		$result_studentname = db_query($sql_studentname, $essayrevision->student_id);

		while ($name = db_fetch_object($result_studentname)) {
			if ($name->name == 'profile_pers_matricnum') $studentmatricnum = $name->value;
		}

		$studentname = get_display_name($essayrevision->student_id);
		 */

		$form['current_num'][$essayrevision->nid] = array('#value' => ($i + 1));
		$form['essay_id'][$essayrevision->nid] = array('#value' => $essayrevision->essay_id);

		//The section below is added to reflect the addition of essay title.
		$form['essay_title'][$essayrevision->nid] = array('#value' => $essayrevision->title);
		
		//The section below is commented out to remove student information.
		//
		// $form['student_name'][$essayrevision->nid] = array('#value' => $studentname);
		// $form['student_matric_num'][$essayrevision->nid] = array('#value' => $studentmatricnum);

		//        $form['submission_num'][$essayrevision->nid] = array('#value' => $essayrevision->revision_number);
		//The section below is commented out to remove module information.
		// $form['group'][$essayrevision->nid] = array('#value' => $essayrevision->group_name);
		//
		if ($essayrevision->status == 2) $status = 'AVAILABLE';
		else if ($essayrevision->status == 1) $status = 'DELETED';
		$form['status'][$essayrevision->nid] = array('#value' => $status);

		if ($essayrevision->bad_essay == 1) {
			$links_ba['bad_essay'] = array(
				'title' => t('X'),
				'href' => "admin/wamp/wamp_essay/".$essayrevision->nid.'.'.
				$essayrevision->vid."/not_bad_essay",
				'html' => TRUE,
			);
			$bad_essay = theme('links', $links_ba);
		}
		else if ($essayrevision->bad_essay == 0) $bad_essay = '';
		$form['bad_essay'][$essayrevision->nid] = array(
			'#type' => 'markup',
			'#attributes' => array('class' => 'center'),
			'#value' => $bad_essay);

		if ($essayrevision->needs_editing == 1) {

			$links_ne['show_essays'] = array(
				'title' => t('X'),
				'href' => "admin/wamp/wamp_essay/".$essayrevision->nid.'.'.
				$essayrevision->vid."/finish_editing",
				'html' => TRUE,
			);
			$needs_editing = theme('links', $links_ne);
		}
		else if ($essayrevision->needs_editing == 0) $needs_editing = '';
		$form['needs_editing'][$essayrevision->nid] = array(
			'#type' => 'markup',
			'#attributes' => array('class' => 'center'),
			'#value' => $needs_editing);

		$form['operations'][$essayrevision->nid] = array(
			'#type' => 'markup',
			'#value' => _wamp_essay_show_essay_submissions_links($essayrevision),
		);

		$form['pager'] = array('#value' => theme('pager', NULL, 50, 0));

		$i++;
	}

	return $form;
}

/**
 * Theme wamp esasy overview form
 *
 * @ingroup themeable
 * @see wamp_essay_show_essay_submissions_form()
 */
function theme_wamp_essay_show_essay_submissions_form($form) {
// Overview table:
	$header = array(
		array('data' => t('No.')),
		array('data' => t('Essay ID'), 'field' => 'r.essay_id'),		

		//The section below is commented out to remove student information.
		//
		// array('data' => t('Student Name')),
		// array('data' => t('Matric #'), 'field' => 'pv.value'),
		//The section below is commented out to remove module information.
		// array('data' => t('Group'), 'field' => 'mg.group_name'),

		//The section below is added to display the essay title.
		array('data' => t('Essay Title'), 'field' => 'nr.title'),

		array('data' => t('Bad Essay'), 'class' => 'center', 'field' => 'r.bad_essay'),
		array('data' => t('Needs Editing'), 'class' => 'center', 'field' => 'r.needs_editing'),
		array('data' => t('Status')),
		// t('Operations'),
		array('data' => t('Operations'), 'class' => 'center'),
	);

	//The section below is commented out to remove student information.
	//Use 'essay_id' in replacement of 'student_name' to identify the essay submissions.
	//
	// if (isset($form['student_name']) && is_array($form['student_name'])) {
	// 	foreach (element_children($form['student_name']) as $key) {
	if (isset($form['essay_id']) && is_array($form['essay_id'])) {
		foreach (element_children($form['essay_id']) as $key) {
		//       print_r($form['modulecode']);
			$rows[] = array(

				drupal_render($form['current_num'][$key]),
				drupal_render($form['essay_id'][$key]),

				//The section below is added to reflect the addition of essay title.
				drupal_render($form['essay_title'][$key]),

				//The section below is commented out to remove student information.
				//
				// drupal_render($form['student_name'][$key]),
				// drupal_render($form['student_matric_num'][$key]),
				//The section below is commented out to remove module information.
				// drupal_render($form['group'][$key]),
				drupal_render($form['bad_essay'][$key]),
				drupal_render($form['needs_editing'][$key]),
				drupal_render($form['status'][$key]),
				drupal_render($form['operations'][$key]),
				// drupal_render($form['operations'][$key]),
			);
		}
	}
	else {
		$rows[] = array(array('data' => t('No essay submissions available.'), 'colspan' => '4'));
	}

	$output .= theme('table', $header, $rows);
	if ($form['pager']['#value']) {
		$output .= drupal_render($form['pager']);
	}

	$output .= drupal_render($form);

	return $output;
}

/**
 * Build the edit and delete links for a essay template.
 *
 * @see wamp_module_list_students_form()
 */
function _wamp_essay_show_essay_submissions_links($record) {

	// $links['edit'] = array(
	// 	'title' => t('Edit'),
	// 	'href' => "node/".$record->nid."/edit",
	// 	'html' => TRUE,
	// );

	if ($record->status == 2)
		$links['delete'] = array(
			'title' => t('Delete'),
			'href' => "admin/wamp/wamp_essay/".$record->nid.".".$record->vid."/delete_essay",
			'html' => TRUE,
		);
	else if ($record->status == 1)
			$links['delete'] = array(
				'title' => t('Restore'),
				'href' => "admin/wamp/wamp_essay/".$record->nid.".".$record->vid."/restore_essay",
				'html' => TRUE,
			);

	//readonly links to other annotator's annotations

	$result = db_query("SELECT annotator_id FROM {wamp_essay_annotation} a
		WHERE a.nid=%d AND a.vid=%d AND a.annotator_id <> 1",
		array($record->nid, $record->vid)
	);
	while ($readonly = db_fetch_object($result)) {
		$name = strtok(get_display_name($readonly->annotator_id), ' ');
		$links['ro_'.$readonly->annotator_id] = array(
			'title' => t($name.'\'s'),
			//The section below is commented out to remove module information.
			// 'href' => 'admin/wamp/wamp_essay/'.$record->nid.'.'.$record->vid.'.'.$record->modulegroup_id.'.'.$readonly->annotator_id.'/view_annotation',
			'href' => 'admin/wamp/wamp_essay/'.$record->nid.'.'.$record->vid.'.'.$readonly->annotator_id.'/view_annotation',
			'html' => TRUE,
		);
	}


	return theme('links', $links);
}

/**
 * Helper function to delete an essay. The essay is not actually deleted but
 * its status is set to 1 which indicates that it is draft status and
 * therefore will not appear in the annotator's list of essays.
 *
 * @see wamp_essay_restore_essay()
 */
function wamp_essay_delete_essay(&$form_state, $record=array()) {

	db_query("UPDATE {wamp_essay_revision} SET status=1 WHERE nid=%d AND vid=%d", array($record['nid'], $record['vid']));
	//The section below is commented out to remove essay template information.
	//
	// $essaytemplate_id = db_result(db_query("SELECT essaytemplate_id FROM {wamp_essay_revision} WHERE nid=%d AND vid=%d", array($record['nid'], $record['vid'])));
	// drupal_set_message('Essay Submission deleted successfully');
	// drupal_goto('admin/wamp/wamp_essay/'.$essaytemplate_id.'/show_essay_submissions');
	drupal_set_message('Essay Submission deleted successfully');
	drupal_goto('admin/wamp/wamp_essay/show_essay_submissions');

}

/**
 * Helper function to restore an essay. Its status is set to 2 which
 * indicates that it is submitted and therefore will  appear in the annotator's
 * list of essays.
 *
 * @see wamp_essay_restore_essay()
 */
function wamp_essay_restore_essay(&$form_state, $record=array()) {

	db_query("UPDATE {wamp_essay_revision} SET status=2 WHERE nid=%d AND vid=%d", array($record['nid'], $record['vid']));
	//The section below is commented out to remove essay template information.
	//
	// $essaytemplate_id = db_result(db_query("SELECT essaytemplate_id FROM {wamp_essay_revision} WHERE nid=%d AND vid=%d", array($record['nid'], $record['vid'])));
	// drupal_set_message('Essay Submission restored successfully');
	// drupal_goto('admin/wamp/wamp_essay/'.$essaytemplate_id.'/show_essay_submissions');
	drupal_set_message('Essay Submission restored successfully');
	drupal_goto('admin/wamp/wamp_essay/show_essay_submissions');

}

/**
 * Helper function to unmark an essay as needing further editing. Its
 * needs_editing status is set to 0 which will disable the warning message when
 * the annotators view the essay.
 */
function wamp_essay_finish_editing(&$form_state, $record=array()) {

	db_query("UPDATE {wamp_essay_revision} SET needs_editing=0 WHERE nid=%d AND vid=%d", array($record['nid'], $record['vid']));
	//The section below is commented out to remove essay template information.
	//
	// $essaytemplate_id = db_result(db_query("SELECT essaytemplate_id FROM {wamp_essay_revision} WHERE nid=%d AND vid=%d", array($record['nid'], $record['vid'])));
	// drupal_set_message('"Needs Editing" flag removed successfully');
	// drupal_goto('admin/wamp/wamp_essay/'.$essaytemplate_id.'/show_essay_submissions');
	drupal_set_message('"Needs Editing" flag removed successfully');
	drupal_goto('admin/wamp/wamp_essay/show_essay_submissions');

}

/**
 * Helper function to unmark an essay as BAD. Its bad essay status is set to 0
 * which will disable the warning message when the annotators view the essay.
 */
function wamp_essay_not_bad_essay(&$form_state, $record=array()) {

	db_query("UPDATE {wamp_essay_revision} SET bad_essay=0 WHERE nid=%d AND vid=%d", array($record['nid'], $record['vid']));
	//The section below is commented out to remove essay template information.
	//
	// $essaytemplate_id = db_result(db_query("SELECT essaytemplate_id FROM {wamp_essay_revision} WHERE nid=%d AND vid=%d", array($record['nid'], $record['vid'])));
	// drupal_set_message('"Bad" flag removed successfully');
	// drupal_goto('admin/wamp/wamp_essay/'.$essaytemplate_id.'/show_essay_submissions');
	drupal_set_message('"Bad" flag removed successfully');
	drupal_goto('admin/wamp/wamp_essay/show_essay_submissions');

}

/**
 * @} End of "ingroup wamp_essay".
 */

?>
