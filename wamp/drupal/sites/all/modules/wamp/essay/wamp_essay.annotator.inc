<?php
// @author Chan Hong Guan chanhg -at- comp.nus.edu.sg
// $Id: wamp_essay.annotator.inc 164 2009-08-17 11:08:32Z chanhg $

/**
 * @ingroup wamp_essay
 * @{
 */

/**
 * Navigation bar (with first, previous, next and last links) for essay
 * annotating and annotation viewing. Mouseover hover text indicating
 * running number and essay id number.
 * @param $type
 *   A string with the content, 'first', 'previous', 'next' or 'last'
 * @param $essay_id
 *   An int representing the node id of the essay
 * @param $count
 *   An int representing the current count number of the essay
 * @param $total
 *   An int representing the total number of essays
 * @return
 *   HTML navigation bar with mouseover text
 */
//The section below is commented out to remove module information & check for spectators.
// function navigate_to_essay($type, $essay_id, $essaytemplate_id, $modulegroup_id, $count = 0, $total = -1, $spectator = false) {
//The section below is commented out to remove essay template information.
// function navigate_to_essay($type, $essay_id, $essaytemplate_id, $count = 0, $total = -1) {
function navigate_to_essay($type, $essay_id, $count = 0, $total = -1) {

	//type-casting for security
	$essay_id = (int) $essay_id;
	//The section below is commented out to remove essay template information.
	// $essaytemplate_id = (int) $essaytemplate_id;
	//The section below is commented out to remove module information.
	// $modulegroup_id = (int) $modulegroup_id;
	$count = (int) $count;
	$total = (int) $total;
	//The section below is commented out to remove check for specators.
	// $spectator = (bool) $spectator;

    switch ($type) {

        case 'first':
            $link_text = '|<< &nbsp;';
            $hover_text = 'First Essay<br>(1)';
            $comparison = 'ORDER BY r.essay_id ASC LIMIT 1';
            break;
        case 'previous' :
            $comparison = 'AND r.essay_id < ' . $essay_id . ' ORDER BY r.essay_id DESC LIMIT 1';
            $link_text = '<<&nbsp;&nbsp;';
            $hover_text = 'Previous Essay<br>'.(($count > 1) ? '('.($count - 1).')': '');
            break;
        case 'next' :
            $comparison = 'AND r.essay_id > ' . $essay_id . ' ORDER BY r.essay_id ASC LIMIT 1';
            $link_text = '>>&nbsp;&nbsp;';
            $hover_text = 'Next Essay<br>'.( (($count+1) < $total) ? '('.($count+1).')' : '');
            break;
        case 'last' :
            $comparison =  ' ORDER BY r.essay_id DESC LIMIT 1';
            $link_text = '>>|';
            $hover_text = 'Last Essay<br>('.($total-1).')';
            break;
        
    }

	//The section below is commented out to remove module information.
	//
	// $result = db_query("SELECT r.nid, r.vid, r.essay_id, r.student_id, mg.modulegroup_id
	// 		FROM {wamp_essay_revision} r, {wamp_module} m, {wamp_essay_template} t,
	// 			{wamp_module_group} mg, {wamp_module_group_student} mgs
    //         WHERE m.module_id = t.module_id AND r.essaytemplate_id = t.essaytemplate_id
	// 			AND m.module_id = mg.module_id AND mg.modulegroup_id = mgs.modulegroup_id
	// 			AND mgs.student_id = r.student_id
	// 			AND mgs.modulegroup_id = ".$modulegroup_id."
    //             AND r.status=2 AND r.essaytemplate_id = " . $essaytemplate_id .
    //         " $comparison");
	//The section below is commented out to remove student information.
	//
	// $result = db_query("SELECT r.nid, r.vid, r.essay_id, r.student_id
	// 		FROM {wamp_essay_revision} r, {wamp_essay_template} t
    //         WHERE r.essaytemplate_id = t.essaytemplate_id
    //             AND r.status=2 AND r.essaytemplate_id = " . $essaytemplate_id .
    //         " $comparison");
	//The section below is commented out to remove essay template information.
	//
	// $result = db_query("SELECT r.nid, r.vid, r.essay_id
	// 		FROM {wamp_essay_revision} r, {wamp_essay_template} t
    //         WHERE r.essaytemplate_id = t.essaytemplate_id
    //             AND r.status=2 AND r.essaytemplate_id = " . $essaytemplate_id .
    //         " $comparison");
	$result = db_query("
		SELECT r.nid, r.vid, r.essay_id
		FROM {wamp_essay_revision} r 
		WHERE r.status = 2
		$comparison");

    $essay = db_fetch_object($result);
    if ($essay != null) {
        $essay_url = base_path()."wamp/wamp_essay/" . $essay->nid . "." . $essay->vid;
		
		//The section below is commented out to remove check for spectators.
		//
		// if ($spectator) {
		// 	$next_annotator_id = db_result(db_query("SELECT annotator_id FROM {wamp_essay_annotation} a
		// 	WHERE a.nid=%d AND a.vid=%d AND a.annotator_id <> 1 ORDER BY annotator_id ASC LIMIT 1",
		// 		array($essay->nid, $essay->vid)
		// 	));
		// 	//The section below is commented out to remove module information.
		// 	// $essay_url .= '.'.$essay->modulegroup_id;
		// 	$essay_url .= '.'.$next_annotator_id."/view_annotation";
		// }
		// else $essay_url .= "/annotate_essay";
		$essay_url .= "/annotate_essay";

		//The section below is commented out to remove module information.
		// $hover_text .= ' Essay ID '.$essay->essay_id.' ('.get_student_matric_num($essay->student_id).')';
		$hover_text .= ' Essay ID '.$essay->essay_id;
        $output = '<span><a href="'.$essay_url.'">'.$link_text.'</a></span>';
        $output .= "<span class=\"hovertip\">$hover_text</span>";
    }
    else {
        $output = '<span>'.$link_text.'</span>';
        $output .= "<span class=\"hovertip\">$hover_text</span>";
    }

    return $output;

}

/**
 * Drop down list of all essays
 *
 * @param $link
 *   Database record object used in breadcrumb in the main user interface method
 * @param $admin
 *   A boolean representing whether the current user is an administrator
 * @return
 *   HTML drop down list of all essays
 */
//The section below is commented out to remove check for spectator.
// function jump_to_essay_dropdownlist($link, $spectator = false) {
function jump_to_essay_dropdownlist($link, $admin = false) {

	//declaring variables
	$returned = array();

	$output = '<div class="dropdown">';
	//The section below is commented out to remove module information.
	//
	// $result = db_query("SELECT r.student_id, r.nid, r.vid, r.essay_id, mg.modulegroup_id
	// 		FROM {wamp_essay_revision} r, {wamp_module} m, {wamp_essay_template} t,
	// 			{wamp_module_group} mg, {wamp_module_group_student} mgs
    //         WHERE m.module_id = t.module_id AND r.essaytemplate_id = t.essaytemplate_id
	// 			AND m.module_id = mg.module_id AND mg.modulegroup_id = mgs.modulegroup_id
	// 			AND mgs.student_id = r.student_id
	// 			AND mgs.modulegroup_id = ".$link->modulegroup_id."
    //             AND r.status=2 AND r.essaytemplate_id = " . $link->essaytemplate_id .
    //         " ORDER BY r.essay_id ASC");
	//The section below is commented out to remove student information.
	//
	// $result = db_query("
	// 	SELECT r.student_id, r.nid, r.vid, r.essay_id
	// 	FROM {wamp_essay_revision} r, {wamp_essay_template} t
	// 	WHERE r.essaytemplate_id = t.essaytemplate_id
	// 		AND r.status=2 
	// 		AND r.essaytemplate_id = " . $link->essaytemplate_id .
	// 		" ORDER BY r.essay_id ASC");
	//The section below is commented out to remove essay template information.
	//
	// $result = db_query("
	// 	SELECT r.nid, r.vid, r.essay_id
	// 	FROM {wamp_essay_revision} r, {wamp_essay_template} t
	// 	WHERE r.essaytemplate_id = t.essaytemplate_id
	// 		AND r.status=2 
	// 		AND r.essaytemplate_id = " . $link->essaytemplate_id .
	// 		" ORDER BY r.essay_id ASC");
	$result = db_query("
		SELECT r.nid, r.vid, r.essay_id
		FROM {wamp_essay_revision} r
		WHERE r.status=2 
		ORDER BY r.essay_id ASC");

	$output .= '<form name="navi">';
	$output .= 'Jump to:&nbsp;&nbsp;';
	// $output .= '<select name="dd" onchange="javascript:change_essay();">';
	$output .= '<select name="dd" id="dd">';
	$uri = $_SERVER['REQUEST_URI'];
	$uri = strtok($uri,'/');
	while ($uri = strtok('/')) {
		if (preg_match('/^[0-9]*\.[0-9]*$/', $uri) == 1) {
			$nid = strtok($uri, '.');
			$vid = strtok('.');
			break;
		}
		//The section below is commented out to reflect the changes in URL format. (4 integers -> 3 integers)
		// else if (preg_match('/^[0-9]*\.[0-9]*\.[0-9]*\.[0-9]*$/', $uri) == 1) {
		else if (preg_match('/^[0-9]*\.[0-9]*\.[0-9]*$/', $uri) == 1) {
			$nid = strtok($uri, '.');
			$vid = strtok('.');
			//The section below is commented out to remove module information.
			// $modulegroup_id = strtok('.');
			$annotator_id = strtok('.');
			break;
		}
	}
	$count = 1;
	$current_count = 0;
	while ($dropdown = db_fetch_object($result)) {
		//The section below is commented out to remove student information.
		// $matric = get_student_matric_num($dropdown->student_id);
		if ($dropdown->nid == $nid && $dropdown->vid == $vid) {
			$selected = 'selected';
			$returned['count'] = $count;
			$returned['id'] = $dropdown->essay_id;
			if (!empty($annotator_id)) $returned['annotator'] = $annotator_id;
		}
		else $selected = '';

		$ids = $dropdown->nid.'.'.$dropdown->vid;
		
		//The section below is commented out to change check for spectators to administrator.
		//
		// if ($spectator) {
		if ($admin) {
			$next_annotator_id = db_result(db_query("SELECT annotator_id FROM {wamp_essay_annotation} a
			WHERE a.nid=%d AND a.vid=%d AND a.annotator_id <> 1 ORDER BY annotator_id ASC LIMIT 1",
				array($dropdown->nid, $dropdown->vid)
			));
			//The section below is commented out to remove module information.
			// $ids .= '.'.$dropdown->modulegroup_id;
			$ids .= '.'.$next_annotator_id;
			$essay_url = $ids."/view_annotation";
		}
		else $essay_url = $ids."/annotate_essay";
		$essay_url = $ids."/annotate_essay";
		//The section below is commented out to remove student information.
		// $output .= '<option value="'.$essay_url.'" '.$selected.'>('.$count.') Essay ID '. $dropdown->essay_id . ' ('. $matric . ')</option>';
		$output .= '<option value="'.$essay_url.'" '.$selected.'>('.$count.') Essay ID '. $dropdown->essay_id.'</option>';

		$count++;
	}
	$output .= '</select></form>';
	$output .= '</div>';

	/*
	$js = "var drupal_base_path = '". base_path() ."';";
	drupal_add_js($js, 'inline');
	*/
	drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'base_path.js');
	
	/*
	drupal_add_js('function change_essay(){

		var $essay = document.navi.dd.options[document.navi.dd.selectedIndex].value;
		var $dest = Drupal.settings.basePath;
		$dest += "wamp/wamp_essay/";
		$dest += $essay;
		//$dest += "/annotate_essay";
		location.href=$dest;
	}', 'inline');
	*/
	drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'change_essay.js');

	drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'dd_event_handler.js');

	$returned['output'] = $output;
	$returned['total'] = $count;

	return $returned;
	
}

/**
 * Drop down list of annotations by others in the specific essay
 *
 * @param $node
 *   node object
 *
 * @return
 *   HTML dropdown list with the annotations made for the specific node (submitted essay)
 */
//The section below is commented out to remove module information.
// function jump_to_other_annotations_dropdownlist($node, $modulegroup_id) {
function jump_to_other_annotations_dropdownlist($node) {

	//declaring variables
	$returned = array();
	global $user;

	$output = '<div class="dropdown_other">';
	$output .= '<form name="navi_other" id="no_bottom_margin">';
	$output .= '<label for="dd">Other Annotations: </label>';
	// $output .= '<select name="dd_other" onchange="javascript:change_annotation();">';
	$output .= '<select name="dd_other" id="dd_other">';
	$output .= '<option value="-1"> -- Please select one -- </option>';
	$uri = $_SERVER['REQUEST_URI'];
	$uri = strtok($uri,'/');
	while ($uri = strtok('/')) {
		//The section below is commented out to reflect the changes in URL format. (4 integers -> 3 integers)
		// if (preg_match('/^[0-9]*\.[0-9]*\.[0-9]*\.[0-9]*$/', $uri) == 1) {
		if (preg_match('/^[0-9]*\.[0-9]*\.[0-9]*$/', $uri) == 1) {
			$nid = strtok($uri, '.');
			$vid = strtok('.');
			//The section below is commented out to remove module information.
			// $modulegroup_id = strtok('.');
			$annotator_id = strtok('.');
			break;
		}
	}
	$result = db_query("SELECT annotator_id FROM {wamp_essay_annotation} a
			WHERE a.nid=%d AND a.vid=%d AND a.annotator_id <> %d AND a.annotator_id <> 1 ORDER BY annotator_id ASC",
				array($node->nid, $node->vid, $user->uid)
			);
	while ($dropdown = db_fetch_object($result)) {
		if ($node->nid == $nid && $node->vid == $vid && $dropdown->annotator_id == $annotator_id) {
			$selected = 'selected';
		}
		else $selected = '';
		$name = strtok(get_display_name($dropdown->annotator_id), ' ');
		//The section below is commented out to remove module information.
		// $output .= '<option value="'.$node->nid.'.'.$node->vid.'.'.$modulegroup_id.'.'.$dropdown->annotator_id.'/view_annotation" '.$selected.'>'.$name.'\'s Annotation</option>';
		$output .= '<option value="'.$node->nid.'.'.$node->vid.'.'.$dropdown->annotator_id.'/view_annotation" '.$selected.'>'.$name.'\'s Annotation</option>';

	}
	if ($selected == 'selected')
			$output .= '<option value="'.$node->nid.'.'.$node->vid.'/annotate_essay" >Own Annotation</option>';
	$output .= '</select></form>';
	//corrected essay
	// $output .= '<div style="float:right"><a href="'.base_path().'wamp/wamp_essay/'.$node->nid.'.'.$node->vid.'.'.$modulegroup_id.'.'.$annotator_id.'/view_corrected_essay">Corrected Essay</a></div>';
	//The section below is commented out to remove module information.
	// $output .= '<div id="corrected_essay_div"><a href="'.base_path().'wamp/wamp_essay/'.$node->nid.'.'.$node->vid.'.'.$modulegroup_id.'.'.$annotator_id.'/view_corrected_essay">Corrected Essay</a></div>';
	$output .= '<div id="corrected_essay_div"><a href="'.base_path().'wamp/wamp_essay/'.$node->nid.'.'.$node->vid.'.'.$annotator_id.'/view_corrected_essay">Corrected Essay</a></div>';
	$output .= '</div>';
	
	/*
	$js = "var drupal_base_path = '". base_path() ."';";
	drupal_add_js($js, 'inline');
	*/
	drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'base_path.js');

	/*
	drupal_add_js('function change_annotation(){

		var $essay = document.navi_other.dd_other.options[document.navi_other.dd_other.selectedIndex];
		var $dest = Drupal.settings.basePath;
		if (location.href.indexOf("/admin") != -1) $dest += "admin/";
		$dest += "wamp/wamp_essay/";
		$dest += $essay.value;
		
		if ($essay.value != "-1") {
			//location.href=$dest;
			document.navi_other.dd_other.selectedIndex = 0;
			if ($essay.text == "Own Annotation") location.href=$dest;
			else window.open($dest, Math.random(), "menubar=yes,scrollbars=yes");
		}
	}', 'inline');
	*/
	drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'change_annotation.js');

	drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'dd_other_event_handler.js');

	$returned['output'] = $output;

	return $returned;

}

/**
 * Form to flag an essay as bad or needing further editing
 *
 * @ingroup forms
 * @see wamp_essay_flag_form_submit()
 */
function wamp_essay_flag_form(&$form_state, $record = array()) {

	//variables
	global $user;
	$msg['bad_essay'] = 'If this essay is generally bad and
				annotations should not be made on it, please check the box on the left
				to notify the WAMP Administrator.';
	$msg['needs_editing'] = 'If this essay has extra information like
				essay title, etc, which should <em>not</em> be in the main
				area of the essay, check the box to notify the WAMP Administrator to remove them.';

	$bad_essay_result = db_query("SELECT bad_essay, bad_essay_uid FROM {wamp_essay_revision} WHERE nid=%d AND vid=%d", array($record->nid, $record->vid));
	$bad_essay = db_fetch_object($bad_essay_result);
	if ($bad_essay->bad_essay == 1) {
		if ($bad_essay->bad_essay_uid == $user->uid) $bad_essay_disabled = 0;
		else $bad_essay_disabled = 1;
	}
	else $bad_essay_disabled = 0;

	$needs_editing_result = db_query("SELECT needs_editing, needs_editing_uid FROM {wamp_essay_revision} WHERE nid=%d AND vid=%d", array($record->nid, $record->vid));
	$needs_editing = db_fetch_object($needs_editing_result);
	if ($needs_editing->needs_editing == 1) {
		if ($needs_editing->needs_editing_uid == $user->uid) $needs_editing_disabled = 0;
		else $needs_editing_disabled = 1;
	}
	else $needs_editing_disabled = 0;

	$form['bad_essay'] = array(
		'#type' => 'checkbox',
		'#title' => '<span>Bad Essay</span>
			<span class="hovertip">'.$msg['bad_essay'].'</span>',
		'#default_value' => $bad_essay->bad_essay,
		'#disabled' => $bad_essay_disabled,
		// '#attributes' => array('onchange' => "form.submit('wamp_essay_flag_form')")
		);

	$form['needs_editing'] = array(
		'#type' => 'checkbox',
		'#title' => '<span>Needs Editing</span>
			<span class="hovertip">'.$msg['needs_editing'].'</span>',
		'#default_value' => $needs_editing->needs_editing,
		'#disabled' => $needs_editing_disabled,
		// '#attributes' => array('onchange' => "form.submit('wamp_essay_flag_form')")
		);
	$form['nid'] = array('#type' => 'hidden', '#value' => $record->nid);
	$form['vid'] = array('#type' => 'hidden', '#value' => $record->vid);

	// $form['go'] = array('#type' => 'submit', '#value' => t('Go'), '#attributes' => array('style' => 'display: none;'));
	$form['go'] = array('#type' => 'submit', '#value' => t('Go'));

	drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'wamp_essay_flag_form.js');

	return $form;
}

function theme_wamp_essay_flag_form(&$form) {

	$output  = '<div>' . drupal_render($form['bad_essay']) . '</div>';
	$output  .= '<div class="sameline">' . drupal_render($form['needs_editing']) . '</div>';
	$output .= drupal_render($form['nid']);
	$output .= drupal_render($form['vid']);
	$output .= drupal_render($form['go']);
	$output .= drupal_render($form);

	return $output;
}


/**
 * Submit handler for form to flag an essay as bad or needing further editing
 *
 * @see wamp_essay_flag_form()
 */
function wamp_essay_flag_form_submit($form, &$form_state) {

	global $user;
	$record = $form_state['values'];
	if ($record['bad_essay'] == 1) {
		db_query("UPDATE {wamp_essay_revision} SET bad_essay=%d, bad_essay_uid=%d WHERE nid=%d AND vid=%d",
			array($record['bad_essay'], $user->uid, $record['nid'], $record['vid']));
	}
	else {
		db_query("UPDATE {wamp_essay_revision} SET bad_essay=NULL, bad_essay_uid=NULL WHERE nid=%d AND vid=%d",
			array($record['nid'], $record['vid']));
	}

	if ($record['needs_editing'] == 1) {
		db_query("UPDATE {wamp_essay_revision} SET needs_editing=%d, needs_editing_uid=%d WHERE nid=%d AND vid=%d",
			array($form_state['values']['needs_editing'], $user->uid, $form_state['values']['nid'], $form_state['values']['vid']));
	}
	else {
		db_query("UPDATE {wamp_essay_revision} SET needs_editing=NULL, needs_editing_uid=NULL WHERE nid=%d AND vid=%d",
			array($record['nid'], $record['vid']));
	}
	
//	drupal_set_message($form_state['values']['bad_essay']);
}

//The section below is commented out to remove check for tutors.
//
// /**
//  * Helper function to determine if the currently logged-in user is a tutor in
//  * the module for which the student submitted the essay.
//  *
//  * @param $nid
//  *   node id
//  * @return
//  *   TRUE if the currently logged-in user is a tutor, otherwise FALSE
//  */
// function is_tutor_for_nid($nid) {
// 	global $user;
// 	$result_sec =
// 		db_query('select mg.modulegroup_id, mgt.tutor_id
// 				from {wamp_essay_revision} r, {wamp_essay_template} t,
// 					{wamp_module} m, {wamp_module_group} mg,
// 					{wamp_module_group_student} mgs, {wamp_module_group_tutor} mgt
// 				where r.nid=%d and t.essaytemplate_id = r.essaytemplate_id
// 					and m.module_id = mg.module_id and mg.modulegroup_id = mgs.modulegroup_id
// 					and mgs.student_id = r.student_id
// 					and mgt.modulegroup_id = mg.modulegroup_id
// 					and mgt.tutor_id=%d', array($nid, $user->uid));

// 	$obj_sec = db_fetch_object($result_sec);
// 	$modulegroup_id = $obj_sec->modulegroup_id;
// 	$tutor_id = $obj_sec->tutor_id;

// 	if (!empty($tutor_id) && $tutor_id == $user->uid) return true;
// 	else return false;
// }

//The section below is commented out to remove check for tutors.
//
// /**
//  * Helper function to determine if the currently logged-in user is a spectator in
//  * the module for which the student submitted the essay.
//  *
//  * @param $nid
//  *   node id
//  * @return
//  *   TRUE if the currently logged-in user is a spectator, otherwise FALSE
//  */
// function is_spectator_for_nid($nid) {

// 	global $user;
// 	$module_id = db_result(
// 		db_query('SELECT t.module_id
// 			FROM {wamp_essay_revision} r, {wamp_essay_template} t
// 			WHERE r.nid=%d AND r.essaytemplate_id = t.essaytemplate_id' , array($nid)));

// 	$spectator_id = db_result(
// 		db_query('select ms.spectator_id
// 				from {wamp_essay_revision} r, {wamp_essay_template} t,
// 					{wamp_module} m, {wamp_module_spectator} ms
// 				where r.nid=%d and t.essaytemplate_id = r.essaytemplate_id
// 					and m.module_id = ms.module_id
// 					and ms.spectator_id=%d and m.module_id=%d',
// 				array($nid, $user->uid, $module_id)));

// 	if (!empty($spectator_id) && $spectator_id == $user->uid) return true;
// 	else return false;
// }

//The section below is commented out to remove module information.
//
// /**
//  * Helper function to determine the module group of an essay which a student
//  * has submitted.
//  *
//  * @param $nid
//  *   node id
//  * @return
//  *   module group id, else NULL
//  */
// function get_modulegroupid_from_nid($nid) {
// 	global $user;
// 	return db_result(db_query('select mg.modulegroup_id
// 				from {wamp_essay_revision} r, {wamp_essay_template} t,
// 					{wamp_module} m, {wamp_module_group} mg,
// 					{wamp_module_group_student} mgs, {wamp_module_group_tutor} mgt
// 				where r.nid=%d and t.essaytemplate_id = r.essaytemplate_id
// 					and t.module_id = m.module_id
// 					and m.module_id = mg.module_id and mg.modulegroup_id = mgs.modulegroup_id
// 					and mgs.student_id = r.student_id
// 					and mgt.modulegroup_id = mg.modulegroup_id
// 					and mgt.tutor_id=%d', array($nid, $user->uid)));
// }

/**
 * Form for annotating essays
 *
 */
function wamp_essay_annotate_form($record = array()) {

	global $user;
	$node = node_load($record['nid'], $record['vid']);
	//The section below is commented out to remove module information.
	// $modulegroup_id = get_modulegroupid_from_nid($node->nid);
	
	//The section below is commented out to remove student information.
	//
    // $student_uid = $node->uid;
    // $student = user_load(array('uid' => $student_uid));
    // $student_name = get_display_name($student->uid);
	// $student_matric = get_student_matric_num($student->uid);

	//The section below is commented out to remove module information.
	//
	// $sql_links = 'SELECT mg.module_id, mg.modulegroup_id, r.essaytemplate_id, 
	// 							r.essay_id, r.bad_essay, r.needs_editing, t.description
	// 					FROM {wamp_essay_revision} r, {wamp_essay_template} t,
	// 							{wamp_module} m, {wamp_module_group} mg
	// 					WHERE r.nid = '.$record['nid'].' AND r.vid = '.$record['vid'].'
	// 							AND r.essaytemplate_id = t.essaytemplate_id
	// 							AND t.module_id = m.module_id
	// 							AND m.module_id = mg.module_id
	// 							AND mg.modulegroup_id = '.$modulegroup_id;
	//The section below is commented out to remove essay template information.
	//
	// $sql_links = 'SELECT r.essaytemplate_id, r.essay_id, r.bad_essay, r.needs_editing, t.description
	// 			FROM {wamp_essay_revision} r, {wamp_essay_template} t
	// 			WHERE r.nid = '.$record['nid'].' AND r.vid = '.$record['vid'].'
	// 				AND r.essaytemplate_id = t.essaytemplate_id';
	//The section below is commented out to reflect the changes in assingment prompt, since essay template (& its description) no longer exist. Replacing with essay title.
	//
	// $sql_links = 'SELECT r.essay_id, r.bad_essay, r.needs_editing
	// 			FROM {wamp_essay_revision} r
	// 			WHERE r.nid = '.$record['nid'].' AND r.vid = '.$record['vid'];
	$sql_links = 'SELECT r.essay_id, r.bad_essay, r.needs_editing, nr.title, nr.body
        FROM {wamp_essay_revision} r, {node_revisions} nr
        WHERE r.nid = nr.nid
			AND r.nid = '.$record['nid'].' AND r.vid = '.$record['vid'];

	$result_links = db_query($sql_links);
	$link = db_fetch_object($result_links);

	$links[] = l(t('Home'),'');

	//The section below is commented out to remove module information.
	//
	// $links[] = l(t('Essays'),'wamp');
	// $links[] = l(t('Modules'),'wamp');
	// $links[] = l(t('Assignments'),'wamp/prompts/'.$link->module_id.'.'.$link->modulegroup_id);
	// $links[] = l(t('Submissions'),'wamp/prompts/essays/'.$link->essaytemplate_id.'.'.$link->modulegroup_id);
	$links[] = l(t('View Essays'),'wamp/view_essays');

	$links[] = l(t('Annotate'),'wamp/wamp_essay/'.$node->nid.'.'.$node->vid.'/annotate_essay');
	drupal_set_breadcrumb($links);

	$output .= 'Your Annotation';

	$dropdown = jump_to_essay_dropdownlist($link);

	//essay navigation
	$output .= '<div id="navi_container"><div class="wamp_top_navi">';
	$output .= '<div id="first_row">';
	$output .= '<div class="fpnl">';
	//The section below is commented out to remove essay template information.
	//
	// $sql_essaytemplateid = "SELECT essaytemplate_id FROM {wamp_essay_revision} WHERE vid=".$record['vid'];
	// $essaytemplate_id = db_result(db_query($sql_essaytemplateid));

	//The section below is commented out to remove module information.
	//
	// $output .= navigate_to_essay('first', $link->essay_id, $essaytemplate_id, $link->modulegroup_id, null);
	// $output .= navigate_to_essay('previous', $link->essay_id, $essaytemplate_id, $link->modulegroup_id, $dropdown['count']);
	// $output .= navigate_to_essay('next', $link->essay_id, $essaytemplate_id, $link->modulegroup_id, $dropdown['count'], $dropdown['total']);
	// $output .= navigate_to_essay('last', $link->essay_id, $essaytemplate_id, $link->modulegroup_id, $dropdown['count'], $dropdown['total']);
	
	//The section below is commented out to remove module information.
	//
	// $output .= navigate_to_essay('first', $link->essay_id, $essaytemplate_id, null);
	// $output .= navigate_to_essay('previous', $link->essay_id, $essaytemplate_id, $dropdown['count']);
	// $output .= navigate_to_essay('next', $link->essay_id, $essaytemplate_id, $dropdown['count'], $dropdown['total']);
	// $output .= navigate_to_essay('last', $link->essay_id, $essaytemplate_id, $dropdown['count'], $dropdown['total']);

	$output .= navigate_to_essay('first', $link->essay_id, null);
	$output .= navigate_to_essay('previous', $link->essay_id, $dropdown['count']);
	$output .= navigate_to_essay('next', $link->essay_id, $dropdown['count'], $dropdown['total']);
	$output .= navigate_to_essay('last', $link->essay_id, $dropdown['count'], $dropdown['total']);

	$output .= "</div>";

	//drop down list navigation to jump to other essays
	$output .= $dropdown['output'];

	//corrected essay
	//The section below is commented out to remove module information.
	// $output .= '<div class="corrected_essay"><a href="'.base_path().'/wamp/wamp_essay/'.$node->nid.'.'.$node->vid.'.'.$link->modulegroup_id.'.'.$user->uid.'/view_corrected_essay">Corrected Essay</a></div>';
	$output .= '<div class="corrected_essay"><a href="'.base_path().'/wamp/wamp_essay/'.$node->nid.'.'.$node->vid.'.'.$user->uid.'/view_corrected_essay">Corrected Essay</a></div>';
	$output .= '</div>';
	//flagging
	$output .=
		'<div class="essay_options">';
		$output .= drupal_get_form('wamp_essay_flag_form', $node);
		$output .= '</div>';

	$output .= '<div class="clear"></div>';
	$output .= '</div>';

	if ($link->bad_essay == 1 || $link->needs_editing == 1) {
		if ($link->bad_essay == 1) {
			$output.= '<div class="bad_essay">
				<span class="warning">This has been marked "BAD"<br></span>
				<span class="fine_print">Please do not annotate this essay until the problem has been resolved.</span>
			</div>';
		}
		if ($link->needs_editing == 1) {
			$output.= '<div class="needs_editing">
				<span class="warning">This has been marked "Needs Editing"<br></span>
				<span class="fine_print">Please do not annotate this essay until the problem has been resolved.</span>
			</div>';
		}
	}

	//essay prompt
	//The section below is commented out to reflect the changes in assingment prompt, since essay template (& its description) no longer exist. Replacing with essay title.
	// $output.= '<div class="essay_prompt">Assignment Prompt:<br>'.$link->description.'</div>';
	$output.= '<div class="essay_prompt">Essay Title:<br>'.$link->title.'</div>';

	//The section below is commented out to remove student information.
	// drupal_set_title('('.$dropdown['count'].') Essay ID '.$dropdown['id'].' (' .$student_matric.')');
	drupal_set_title('('.$dropdown['count'].') Essay ID '.$dropdown['id']);

	//load firebug only on testing system
	if (variable_get('wamp_system', '') == 'testing') {
		$js = '/js/';
		drupal_add_js(drupal_get_path('module', 'wamp_essay').$js.'firebug-lite.js');
		// drupal_add_js('firebug.env.debug = false', 'inline');
		drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'firebug_env_debug.js');
		// drupal_add_js('firebug.env.height = 200', 'inline');
		drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'firebug_env_height.js');

	}
	else $js = '/js/minified/';

	//ensure that ierange is loaded properly across different versions of site
	drupal_set_html_head("<!--[if IE]>");
	drupal_set_html_head('<script type=text/javascript src="'.base_path().drupal_get_path('module','wamp_essay').$js.'ierange-m2.custom.js"></script>');
	drupal_set_html_head("<![endif]-->");

	drupal_add_js(drupal_get_path('module', 'wamp_essay').$js.'annotator.js');
	drupal_add_js(drupal_get_path('module', 'wamp_essay').$js.'ui.js');
	// drupal_add_js("var cur_loc = location.pathname; var lastSlash = cur_loc.lastIndexOf('/'); var annotationsURL = cur_loc.substr(0, lastSlash); annotationsURL += '/annotation_save';", 'inline');
	drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'annotation_save.js');
	
	//configure HTML based on the selected error type
	// 1) CoNLL-2014
	$ui_html = 'ui.html';
	// 2) ERRANT
	if (variable_get("error_type", 0) == 1) {
		$ui_html = 'ui_errant.html';
	};
	$js_setting = array(
		'wamp_essay' => array(
			'ui_html' => $ui_html,
		),
	);
	drupal_add_js($js_setting, 'setting');
	// drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'first_slash.js');
	// drupal_add_js("var firstSlash = cur_loc.indexOf('/',1); var uiURL = cur_loc.substr(0, firstSlash); uiURL += '/wamp_includes/ui.html';", 'inline');
	/*
	drupal_add_js("$(document).bind('ready', function() {
		var ui = new EditUI(uiURL, annotationsURL);
	});", 'inline');
	*/
	drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'initialize_editui.js');

	drupal_add_css(drupal_get_path('module', 'wamp_essay').'/css/annotator.css');
	drupal_add_css(drupal_get_path('module', 'wamp').'/drupal.css');
	$output .= '<div id="wamp_body">'.$node->body.'</div>';

	//The section below is commented out to remove check for tutors.
	//
	// if (is_tutor_for_nid($node->nid)) {

	// 	$sql_links = 'SELECT mg.module_id, mg.modulegroup_id, r.essaytemplate_id, 
	// 					r.essay_id, r.bad_essay, r.needs_editing, t.description
	// 				FROM {wamp_essay_revision} r, {wamp_essay_template} t,
	// 					{wamp_module} m, {wamp_module_group} mg
	// 				WHERE r.nid = '.$record['nid'].' AND r.vid = '.$record['vid'].'
	// 					AND r.essaytemplate_id = t.essaytemplate_id
	// 					AND t.module_id = m.module_id
	// 					AND m.module_id = mg.module_id
	// 					AND mg.modulegroup_id = '.$modulegroup_id;
	// 	$result_links = db_query($sql_links);
	// 	$link = db_fetch_object($result_links);

	// 	$links[] = l(t('Home'),'');
	// 	$links[] = l(t('Modules'),'wamp');
	// 	$links[] = l(t('Assignments'),'wamp/prompts/'.$link->module_id.'.'.$link->modulegroup_id);
	// 	$links[] = l(t('Submissions'),'wamp/prompts/essays/'.$link->essaytemplate_id.'.'.$link->modulegroup_id);
	// 	$links[] = l(t('Annotate'),'wamp/wamp_essay/'.$node->nid.'.'.$node->vid.'/annotate_essay');
	// 	drupal_set_breadcrumb($links);

	// 	$output .= 'Your Annotation';

	// 	$dropdown = jump_to_essay_dropdownlist($link);

	// 	//essay navigation
	// 	$output .= '<div id="navi_container"><div class="wamp_top_navi">';
	// 	$output .= '<div id="first_row">';
	// 	$output .= '<div class="fpnl">';
	// 	$sql_essaytemplateid = "SELECT essaytemplate_id FROM {wamp_essay_revision} WHERE vid=".$record['vid'];
	// 	$essaytemplate_id = db_result(db_query($sql_essaytemplateid));
	// 	$output .= navigate_to_essay('first', $link->essay_id, $essaytemplate_id, $link->modulegroup_id, null);
	// 	$output .= navigate_to_essay('previous', $link->essay_id, $essaytemplate_id, $link->modulegroup_id, $dropdown['count']);
	// 	$output .= navigate_to_essay('next', $link->essay_id, $essaytemplate_id, $link->modulegroup_id, $dropdown['count'], $dropdown['total']);
	// 	$output .= navigate_to_essay('last', $link->essay_id, $essaytemplate_id, $link->modulegroup_id, $dropdown['count'], $dropdown['total']);
	// 	$output .= "</div>";

	// 	//drop down list navigation to jump to other essays
	// 	$output .= $dropdown['output'];

	// 	//corrected essay
	// 	$output .= '<div class="corrected_essay"><a href="'.base_path().'/wamp/wamp_essay/'.$node->nid.'.'.$node->vid.'.'.$link->modulegroup_id.'.'.$user->uid.'/view_corrected_essay">Corrected Essay</a></div>';
	// 	$output .= '</div>';
	// 	//flagging
	// 	$output .=
	// 		'<div class="essay_options">';
	// 		$output .= drupal_get_form('wamp_essay_flag_form', $node);
	// 		$output .= '</div>';

	// 	$output .= '<div class="clear"></div>';
	// 	$output .= '</div>';

	// 	if ($link->bad_essay == 1 || $link->needs_editing == 1) {
	// 		if ($link->bad_essay == 1) {
	// 			$output.= '<div class="bad_essay">
	// 				<span class="warning">This has been marked "BAD"<br></span>
	// 				<span class="fine_print">Please do not annotate this essay until the problem has been resolved.</span>
	// 			</div>';
	// 		}
	// 		if ($link->needs_editing == 1) {
	// 			$output.= '<div class="needs_editing">
	// 				<span class="warning">This has been marked "Needs Editing"<br></span>
	// 				<span class="fine_print">Please do not annotate this essay until the problem has been resolved.</span>
	// 			</div>';
	// 		}
	// 	}

	// 	//essay prompt
	// 	$output.= '<div class="essay_prompt">Assignment Prompt:<br>'.$link->description.'</div>';

	// 	drupal_set_title('('.$dropdown['count'].') Essay ID '.$dropdown['id'].' (' .$student_matric.')');

	// 	//load firebug only on testing system
	// 	if (variable_get('wamp_system', '') == 'testing') {
	// 		$js = '/js/';
	// 		drupal_add_js(drupal_get_path('module', 'wamp_essay').$js.'firebug-lite.js');
	// 		// drupal_add_js('firebug.env.debug = false', 'inline');
	// 		drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'firebug_env_debug.js');
	// 		// drupal_add_js('firebug.env.height = 200', 'inline');
	// 		drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'firebug_env_height.js');

	// 	}
	// 	else $js = '/js/minified/';

	// 	//ensure that ierange is loaded properly across different versions of site
	// 	drupal_set_html_head("<!--[if IE]>");
	// 	drupal_set_html_head('<script type=text/javascript src="'.base_path().drupal_get_path('module','wamp_essay').$js.'ierange-m2.custom.js"></script>');
	// 	drupal_set_html_head("<![endif]-->");

	// 	drupal_add_js(drupal_get_path('module', 'wamp_essay').$js.'annotator.js');
	// 	drupal_add_js(drupal_get_path('module', 'wamp_essay').$js.'ui.js');
	// 	// drupal_add_js("var cur_loc = location.pathname; var lastSlash = cur_loc.lastIndexOf('/'); var annotationsURL = cur_loc.substr(0, lastSlash); annotationsURL += '/annotation_save';", 'inline');
	// 	drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'annotation_save.js');
		
	// 	//configure HTML based on the selected error type
	// 	// 1) CoNLL-2014
	// 	$ui_html = 'ui.html';
	// 	// 2) ERRANT
	// 	if (variable_get("error_type", 1)) {
	// 		$ui_html = 'ui_errant.html';
	// 	};
	// 	$js_setting = array(
	// 		'wamp_essay' => array(
	// 			'ui_html' => $ui_html,
	// 		),
	// 	);
	// 	drupal_add_js($js_setting, 'setting');
	// 	// drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'first_slash.js');
	// 	// drupal_add_js("var firstSlash = cur_loc.indexOf('/',1); var uiURL = cur_loc.substr(0, firstSlash); uiURL += '/wamp_includes/ui.html';", 'inline');
	// 	/*
	// 	drupal_add_js("$(document).bind('ready', function() {
	// 		var ui = new EditUI(uiURL, annotationsURL);
	// 	});", 'inline');
	// 	*/
	// 	drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'initialize_editui.js');

	// 	drupal_add_css(drupal_get_path('module', 'wamp_essay').'/css/annotator.css');
	// 	drupal_add_css(drupal_get_path('module', 'wamp').'/drupal.css');
	// 	$output .= '<div id="wamp_body">'.$node->body.'</div>';
		
	// }
	// else {
	// 	$output = 'You are not authorised to annotate this essay';
	// }
	
	return $output;
}

/**
 * Form for viewing annotated essays
 *
 */
function wamp_essay_view_annotation_form($record = array()) {

	global $user;
	
	$node = node_load($record['nid'], $record['vid']);

	/**
	 * The section below is commented out to remove module information.
	 * 
	$sql_links = 'SELECT mg.module_id, mg.modulegroup_id, r.essaytemplate_id, r.essay_id
							, t.description
						FROM {wamp_essay_revision} r, {wamp_essay_template} t,
							{wamp_module} m, {wamp_module_group} mg
						WHERE r.nid = '.$record['nid'].' AND r.vid = '.$record['vid'].'
							AND r.essaytemplate_id = t.essaytemplate_id
							AND t.module_id = m.module_id
							AND m.module_id = mg.module_id
							AND mg.modulegroup_id = '.$record['modulegroup_id'];
		$result_links = db_query($sql_links);
		$link = db_fetch_object($result_links);
	 */
	//The section below is commented out to remove essay template information.
	//
	// $sql_links = '
	// 	SELECT r.essaytemplate_id, r.essay_id, t.description
	// 	FROM {wamp_essay_revision} r, {wamp_essay_template} t
	// 	WHERE r.nid = '.$record['nid'].' 
	// 		AND r.vid = '.$record['vid'].'
	// 		AND r.essaytemplate_id = t.essaytemplate_id';

	//The section below is commented out to reflect the changes in assingment prompt, since essay template (& its description) no longer exist. Replacing with essay title.
	//
	// $sql_links = '
	// 	SELECT r.essay_id
	// 	FROM {wamp_essay_revision} r
	// 	WHERE r.nid = '.$record['nid'].' 
	// 		AND r.vid = '.$record['vid'];
	$sql_links = 'SELECT r.essay_id, nr.title, nr.body
        FROM {wamp_essay_revision} r, {node_revisions} nr
        WHERE r.nid = nr.nid
			AND r.nid = '.$record['nid'].' AND r.vid = '.$record['vid'];

	$result_links = db_query($sql_links);
	$link = db_fetch_object($result_links);

	//The section below is commented out to remove check for spectator.
	// if ((is_spectator_for_nid($node->nid) && !empty($link)) || $user->uid == 1) {
	if ($user->uid == 1) {
		$dropdown = jump_to_essay_dropdownlist($link, true);
	//
		//The section below is commented out to remove check for administrator. Since spectator no longer exists, the user is administrator by default at this point.
		//
		// $links[] = l(t('Home'),'');
		// $links[] = l(t('Modules'),'wamp');
		// $links[] = l(t('Assignments'),'wamp/prompts/'.$link->module_id.'.'.$link->modulegroup_id);
		// $links[] = l(t('Submissions'),'wamp/prompts/essays/'.$link->essaytemplate_id.'.'.$link->modulegroup_id);
		// $links[] = l(t('View Annotation'),'wamp/wamp_essay/'.$node->nid.'.'.$node->vid.'.'.$link->modulegroup_id.'.'.$dropdown['annotator'].'/view_annotation');
		// $bc = drupal_get_breadcrumb();
		// $bc[] = l(t('Submissions'), 'admin/wamp/wamp_essay/'.$link->essaytemplate_id.'/show_essay_submissions');
		// if ($user->uid == 1)
		// 	drupal_set_breadcrumb($bc);
		// else
		// 	drupal_set_breadcrumb($links);
		$bc = drupal_get_breadcrumb();
		//The section below is commented out to remove essay template information.
		// $bc[] = l(t('Submissions'), 'admin/wamp/wamp_essay/'.$link->essaytemplate_id.'/show_essay_submissions');
		$bc[] = l(t('Submissions'), 'admin/wamp/wamp_essay/show_essay_submissions');
		drupal_set_breadcrumb($bc);

		$output .= get_display_name($record['annotator']).'\'s Annotation';

		$output .= '<div id="navi_container"><div class="wamp_top_navi"><div id="first_row">';
		//The section below is commented out because spectator no longer exists.
		// if ($user->uid != 1) {
		// 	//essay navigation		
		// 	$output .=	'<div class="fpnl">';
		// 	$sql_essaytemplateid = "SELECT essaytemplate_id FROM {wamp_essay_revision} WHERE vid=$node->vid";
		// 	$essaytemplate_id = db_result(db_query($sql_essaytemplateid));

		// 	//The section below is commented out to remove module information.
		// 	//
		// 	// $output .= navigate_to_essay('first', $link->essay_id, $essaytemplate_id, $link->modulegroup_id, 0, -1, true);
		// 	// $output .= navigate_to_essay('previous', $link->essay_id, $essaytemplate_id, $link->modulegroup_id, $dropdown['count'], -1, true);
		// 	// $output .= navigate_to_essay('next', $link->essay_id, $essaytemplate_id, $link->modulegroup_id, $dropdown['count'], $dropdown['total'], true);
		// 	// $output .= navigate_to_essay('last', $link->essay_id, $essaytemplate_id, $link->modulegroup_id, $dropdown['count'], $dropdown['total'], true);

		// 	$output .= navigate_to_essay('first', $link->essay_id, $essaytemplate_id, 0, -1, true);
		// 	$output .= navigate_to_essay('previous', $link->essay_id, $essaytemplate_id, $dropdown['count'], -1, true);
		// 	$output .= navigate_to_essay('next', $link->essay_id, $essaytemplate_id, $dropdown['count'], $dropdown['total'], true);
		// 	$output .= navigate_to_essay('last', $link->essay_id, $essaytemplate_id, $dropdown['count'], $dropdown['total'], true);
		// 	$output .= "</div>";

		// 	$output .= $dropdown['output'];
		// }

		//drop down list navigation to jump to other annotations of the same essay
		//The section below is commented out to remove module information.
		// $dropdown_other = jump_to_other_annotations_dropdownlist($node, $link->modulegroup_id);
		$dropdown_other = jump_to_other_annotations_dropdownlist($node);
		$output .= $dropdown_other['output'];

		//corrected essay
//		$output .= '<div style="float:right"><a href="'.base_path().'/wamp/wamp_essay/'.$node->nid.'.'.$node->vid.'.'.$link->modulegroup_id.'.'.$user->uid.'/view_corrected_essay">Corrected Essay</a></div>';
//		$output .= '</div>';

		// $output .= '<div style="clear:both"></div>';
		$output .= '<div id="corrected_essay_margin"></div>';
		$output .= '</div>';

		//essay prompt
		//The section below is commented out to reflect the changes in assingment prompt, since essay template (& its description) no longer exist. Replacing with essay title.
		// $output.= '<div class="essay_prompt">Assignment Prompt:<br>'.$link->description.'</div>';
		$output.= '<div class="essay_prompt">Essay Title:<br>'.$link->title.'</div>';

		//The section below is commented out to remove student information.
		// drupal_set_title('('.$dropdown['count'].') Essay ID '.$dropdown['id'].' (' .get_student_matric_num($node->uid).')');
		drupal_set_title('('.$dropdown['count'].') Essay ID '.$dropdown['id']);

		//load firebug only on testing system
		if (variable_get('wamp_system', '') == 'testing') {
			$js = '/js/';
			drupal_add_js(drupal_get_path('module', 'wamp_essay').$js.'firebug-lite.js');
			// drupal_add_js('firebug.env.debug = false', 'inline');
			drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'firebug_env_debug.js');
			// drupal_add_js('firebug.env.height = 200', 'inline');
			drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'firebug_env_height.js');
		}
		else $js = '/js/minified/';

		//ensure that ierange is loaded properly across different versions of site
		drupal_set_html_head("<!--[if IE]>");
		drupal_set_html_head('<script type=text/javascript src="'.base_path().drupal_get_path('module', 'wamp_essay').$js.'ierange-m2.custom.js"></script>');
		drupal_set_html_head("<![endif]-->");

		drupal_add_js(drupal_get_path('module', 'wamp_essay').$js.'annotator.js');
		drupal_add_js(drupal_get_path('module', 'wamp_essay').$js.'ui.js');
		// drupal_add_js("var cur_loc = location.pathname; var lastSlash = cur_loc.lastIndexOf('/'); var annotationsURL = cur_loc.substr(0, lastSlash); annotationsURL += '/annotation_readonly';", 'inline');
		drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'annotation_readonly.js');
		
		//configure HTML based on the selected error type
		// 1) CoNLL-2014
		$ui_html = 'ui.html';
		// 2) ERRANT
		if (variable_get("error_type", 0) == 1) {
			$ui_html = 'ui_errant.html';
		};
		$js_setting = array(
			'wamp_essay' => array(
				'ui_html' => $ui_html,
			),
		);
		drupal_add_js($js_setting, 'setting');
		// drupal_add_js("var firstSlash = cur_loc.indexOf('/',1); var uiURL = cur_loc.substr(0, firstSlash); uiURL += '/wamp_includes/ui.html';", 'inline');
		// drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'first_slash.js');
		/*
		drupal_add_js("$(document).bind('ready', function() {
			var ui = new ViewUI(uiURL, annotationsURL);
		});", 'inline');
		*/
		drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'initialize_viewui.js');
		
		drupal_add_css(drupal_get_path('module', 'wamp_essay').'/css/annotator.css');
		drupal_add_css(drupal_get_path('module', 'wamp').'/drupal.css');
		$output .= '<div id="wamp_body">'.$node->body.'</div>';
	}
	else {
		$output = "You are not authorised to view this annotation.";
	}
    return $output;
}

/**
 * Form for viewing corrected essays
 *
 */
function wamp_essay_view_corrected_essay_form($record = array()) {

	global $user;
	$node = node_load($record['nid'], $record['vid']);

	/**
	 * The section below is commented out to remove module information.
	 * 
	$sql_links = 'SELECT mg.module_id, mg.modulegroup_id, r.essaytemplate_id, r.essay_id, t.description
						FROM {wamp_essay_revision} r, {wamp_essay_template} t,
							{wamp_module} m, {wamp_module_group} mg
						WHERE r.nid = '.$record['nid'].' AND r.vid = '.$record['vid'].'
							AND r.essaytemplate_id = t.essaytemplate_id
							AND t.module_id = m.module_id
							AND m.module_id = mg.module_id
							AND mg.modulegroup_id = '.$record['modulegroup_id'];
		$result_links = db_query($sql_links);
		$link = db_fetch_object($result_links);
	 */
	//The section below is commented out to remove essay template information.
	//
	// $sql_links = 'SELECT r.essaytemplate_id, r.essay_id, t.description
	// 					FROM {wamp_essay_revision} r, {wamp_essay_template} t
	// 					WHERE r.nid = '.$record['nid'].' AND r.vid = '.$record['vid'].'
	// 						AND r.essaytemplate_id = t.essaytemplate_id';
	//The section below is commented out to reflect the changes in assingment prompt, since essay template (& its description) no longer exist. Replacing with essay title.
	//
	// $sql_links = '
	// 	SELECT r.essay_id
	// 	FROM {wamp_essay_revision} r
	// 	WHERE r.nid = '.$record['nid'].' 
	// 		AND r.vid = '.$record['vid'];
	$sql_links = 'SELECT r.essay_id, nr.title, nr.body
        FROM {wamp_essay_revision} r, {node_revisions} nr
        WHERE r.nid = nr.nid
			AND r.nid = '.$record['nid'].' AND r.vid = '.$record['vid'];

	$result_links = db_query($sql_links);
	$link = db_fetch_object($result_links);

	//The section below is commented out to remove check for spectator.
	//
	// $is_spectator = is_spectator($link->module_id);
	// 	$dropdown = jump_to_essay_dropdownlist($link, $is_spectator);
	$dropdown = jump_to_essay_dropdownlist($link);

	$links[] = l(t('Home'),'');
	
	//The section below is commented out to remove module information.
	//
	// $links[] = l(t('Modules'),'wamp');
	// $links[] = l(t('Assignments'),'wamp/prompts/'.$link->module_id.'.'.$link->modulegroup_id);
	// $links[] = l(t('Submissions'),'wamp/prompts/essays/'.$link->essaytemplate_id.'.'.$link->modulegroup_id);
	$links[] = l(t('View Essays'),'wamp/view_essays');

	//The section below is commented out to remove check for spectator.
	//
	// if ($is_spectator)
	// 	//The section below is commented out to remove module information.
	// 	// $links[] = l(t('View Annotation'),'wamp/wamp_essay/'.$node->nid.'.'.$node->vid.'.'.$link->modulegroup_id.'.'.$dropdown['annotator'].'/view_annotation');
	// 	$links[] = l(t('View Annotation'),'wamp/wamp_essay/'.$node->nid.'.'.$node->vid.'.'.$dropdown['annotator'].'/view_annotation');
	// else
	// 	$links[] = l(t('Annotate'),'wamp/wamp_essay/'.$node->nid.'.'.$node->vid.'/annotate_essay');
	$links[] = l(t('Annotate'),'wamp/wamp_essay/'.$node->nid.'.'.$node->vid.'/annotate_essay');

	$bc = drupal_get_breadcrumb();
	//The section below is commented out to remove essay template information.
	// $bc[] = l(t('Submissions'), 'admin/wamp/wamp_essay/'.$link->essaytemplate_id.'/show_essay_submissions');
	$bc[] = l(t('Submissions'), 'admin/wamp/wamp_essay/show_essay_submissions');
	if ($user->uid == 1)
		drupal_set_breadcrumb($bc);
	else
		drupal_set_breadcrumb($links);


	$output .= get_display_name($record['annotator']).'\'s Annotation';

	if ($user->uid != 1) {
		//essay navigation
		$output .= '<div id="navi_container"><div class="wamp_top_navi">';
		$output .= '<div class="fpnl">';
		//The section below is commented out to remove essay template information.
		//
		// $sql_essaytemplateid = "SELECT essaytemplate_id FROM {wamp_essay_revision} WHERE vid=$node->vid";
		// $essaytemplate_id = db_result(db_query($sql_essaytemplateid));

		//The section below is commented out to remove module information.
		//
		// $output .= navigate_to_essay('first', $link->essay_id, $essaytemplate_id, $link->modulegroup_id, 0, -1, $is_spectator);
		// $output .= navigate_to_essay('previous', $link->essay_id, $essaytemplate_id, $link->modulegroup_id, $dropdown['count'], -1, $is_spectator);
		// $output .= navigate_to_essay('next', $link->essay_id, $essaytemplate_id, $link->modulegroup_id, $dropdown['count'], $dropdown['total'], $is_spectator);
		// $output .= navigate_to_essay('last', $link->essay_id, $essaytemplate_id, $link->modulegroup_id, $dropdown['count'], $dropdown['total'], $is_spectator);

		//The section below is commented out to remove essay template information.
		//
		// $output .= navigate_to_essay('first', $link->essay_id, $essaytemplate_id, 0, -1);
		// $output .= navigate_to_essay('previous', $link->essay_id, $essaytemplate_id, $dropdown['count'], -1);
		// $output .= navigate_to_essay('next', $link->essay_id, $essaytemplate_id, $dropdown['count'], $dropdown['total']);
		// $output .= navigate_to_essay('last', $link->essay_id, $essaytemplate_id, $dropdown['count'], $dropdown['total']);

		$output .= navigate_to_essay('first', $link->essay_id, 0, -1);
		$output .= navigate_to_essay('previous', $link->essay_id, $dropdown['count'], -1);
		$output .= navigate_to_essay('next', $link->essay_id, $dropdown['count'], $dropdown['total']);
		$output .= navigate_to_essay('last', $link->essay_id, $dropdown['count'], $dropdown['total']);

		$output .= "</div>";

		$output .= $dropdown['output'];
	}

	//The section below is commented out to remove the check for tutors & spectators
	//
	// //drop down list navigation to jump to other annotations of the same essay
	// if (is_spectator_for_nid($node->nid)) {
	// 	//The section below is commented out to remove module informaiton.
	// 	// $dropdown_other = jump_to_other_annotations_dropdownlist($node, $link->modulegroup_id);
	// 	$dropdown_other = jump_to_other_annotations_dropdownlist($node);
	// 	$output .= $dropdown_other['output'];
	// }
	// else if (is_tutor_for_nid($node->nid)) {
	// 	//corrected essay
	// 	$output .= '<div class="corrected_essay"><a href="'.base_path().'wamp/wamp_essay/'.$node->nid.'.'.$node->vid.'.'.$link->modulegroup_id.'.'.$user->uid.'/view_corrected_essay">Corrected Essay</a></div>';
	// 	$output .= '</div>';
	// }
	if ($user->uid != 1) {
		//corrected essay
		//The section below is commented out to remove module information.
		// $output .= '<div class="corrected_essay"><a href="'.base_path().'wamp/wamp_essay/'.$node->nid.'.'.$node->vid.'.'.$link->modulegroup_id.'.'.$user->uid.'/view_corrected_essay">Corrected Essay</a></div>';
		$output .= '<div class="corrected_essay"><a href="'.base_path().'wamp/wamp_essay/'.$node->nid.'.'.$node->vid.'.'.$user->uid.'/view_corrected_essay">Corrected Essay</a></div>';
		$output .= '</div>';
	}

	// $output .= '<div style="clear:both"></div>';
	$output .= '<div id="corrected_essay_margin"></div>';
	$output .= '</div>';

	//essay prompt
	//The section below is commented out to reflect the changes in assingment prompt, since essay template (& its description) no longer exist. Replacing with essay title.
	// $output.= '<div class="essay_prompt">Assignment Prompt:<br>'.$link->description.'</div>';
	$output.= '<div class="essay_prompt">Essay Title:<br>'.$link->title.'</div>';

	//load firebug only on testing system
	if (variable_get('wamp_system', '') == 'testing') {
		$js = '/js/';
		drupal_add_js(drupal_get_path('module', 'wamp_essay').$js.'firebug-lite.js');
		// drupal_add_js('firebug.env.debug = false', 'inline');
		drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'firebug_env_debug.js');
		// drupal_add_js('firebug.env.height = 200', 'inline');
		drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'firebug_env_height.js');
	}
	else $js = '/js/minified/';

	//ensure that ierange is loaded properly across different versions of site
	drupal_set_html_head("<!--[if IE]>");
	drupal_set_html_head('<script type=text/javascript src="'.base_path().drupal_get_path('module','wamp_essay').$js.'ierange-m2.custom.js"></script>');
	drupal_set_html_head("<![endif]-->");

	//The section below is commented out to remove student information.
	// drupal_set_title('('.$dropdown['count'].') Essay ID '.$dropdown['id'].' (' .get_student_matric_num($node->uid).')');
	drupal_set_title('('.$dropdown['count'].') Essay ID '.$dropdown['id']);

	drupal_add_js(drupal_get_path('module', 'wamp_essay').$js.'annotator.js');
	drupal_add_js(drupal_get_path('module', 'wamp_essay').$js.'corrector.js');
	/*
	drupal_add_js("var cur_loc = location.pathname; var lastSlash = cur_loc.lastIndexOf('/'); var annotationsURL = cur_loc.substr(0, lastSlash); annotationsURL += '/annotation_readonly';
	$(document).bind('ready', function() {
		var corrector = new Corrector(annotationsURL);
	});", 'inline');
	*/
	drupal_add_js(drupal_get_path('module', 'wamp_essay').'/js/'.'initialize_corrector.js');
	drupal_add_css(drupal_get_path('module', 'wamp_essay').'/css/annotator.css');
	drupal_add_css(drupal_get_path('module', 'wamp').'/drupal.css');
	$output .= '<div id="wamp_body">'.$node->body.'</div>';

    return $output;
}

/**
 * Handler to accept ajax requests from the annotating page.
 *
 * Accepted operations: save and load.
 * 
 * Badly-formed XML data whether from the database in load operations or from
 * the xml attribute in the save operations will result in HTTP 508 error.
 *
 * @param $record
 *    nid and vid loaded using menu and schema api
 * @return
 *    essay annotation in xml format
 */
function wamp_essay_annotation_save_ajax($record = array()) {

    global $user;
	$record = node_load($record['nid'], $record['vid']);

	//disable xml errors
	error_reporting(0);

    $op = $_REQUEST['operation'];
    if (!empty($op) && $op == 'load') {

        //unique annotation per annotator per essay
        $sql_find = "SELECT nid from {wamp_essay_annotation} where nid=%d and vid=%d and annotator_id=%d";
        $nid = db_result(db_query($sql_find, array($record->nid, $record->vid, $user->uid)));
        
        if (empty($nid)) {
            echo('<annotations></annotations>');
        }
        else {

            $sql_xml = "SELECT annotations from {wamp_essay_annotation} where nid=%d and vid=%d and annotator_id=%d";
            $xmlstr = db_result(db_query($sql_xml, array($record->nid, $record->vid, $user->uid)));
			
			//return HTTP Error 508 for badly-formed xml
			// if( ! $xml = simplexml_load_string($xmlstr) )
			$xml = simplexml_load_string($xmlstr);
 			if( $xml === FALSE )
			{
				header("HTTP/1.0 508 Invalid XML");
			}
            else {
				echo $xmlstr;
			}
        }
    } else if ($op == 'save') {

            $sql_find = "SELECT nid from {wamp_essay_annotation} where nid=%d and vid=%d and annotator_id=%d";
            $nid = db_result(db_query($sql_find, array($record->nid, $record->vid, $user->uid)));

			$xmlstr = $_REQUEST['xml'];

            if (empty($nid)) {				

				//return HTTP Error 508 for badly-formed xml
				// if( ! $xml = simplexml_load_string($xmlstr) )
				$xml = simplexml_load_string($xmlstr);
 				if( $xml === FALSE )
				{
					header("HTTP/1.0 508 Invalid XML");
				}
				else {

					$sql_insert = "INSERT INTO {wamp_essay_annotation} (nid,vid,annotator_id, annotations)
					VALUES(%d, %d, %d, '%s')";
					db_query($sql_insert, array($record->nid, $record->vid, $user->uid, $xmlstr));
					echo $xmlstr;
					
				}
            }
            else {

				//return HTTP Error 508 for badly-formed xml
				// if( ! $xml = simplexml_load_string($xmlstr) )
				$xml = simplexml_load_string($xmlstr);
				if( $xml === FALSE )
				{
					header("HTTP/1.0 508 Invalid XML");
				}
				else {
					$sql_update = "UPDATE {wamp_essay_annotation} SET annotations='%s' WHERE
					nid=%d and vid=%d and annotator_id=%d";
					db_query($sql_update, array($xmlstr, $record->nid, $record->vid, $user->uid));
					echo $xmlstr;
				}
            }
        }
}

/**
 * Handler to accept ajax requests from the annotating page
 * Accepted operations: load only
 *
 * Badly-formed XML data whether from the database in load operations 
 * will result in HTTP 508 error.
 *
 * @param $record
 *    nid and vid loaded using menu and schema api
 */
function wamp_essay_annotation_ro_ajax($record = array()) {

    $op = $_REQUEST['operation'];

    if (!empty($op) && $op == 'load') {
		$sql_xml = "SELECT annotations from {wamp_essay_annotation} where nid=%d and vid=%d and annotator_id=%d";
		$xmlstr = db_result(db_query($sql_xml, array($record['nid'], $record['vid'], $record['annotator'])));
		
		//return HTTP Error 508 for badly-formed xml
		// if( ! $xml = simplexml_load_string($xmlstr) )
		$xml = simplexml_load_string($xmlstr);
		if( $xml === FALSE )
		{
			header("HTTP/1.0 508 Invalid XML");
		}
		else {
			echo $xmlstr;
		}
	}
}

/**
 * @} End of "ingroup wamp_essay".
 */

?>
