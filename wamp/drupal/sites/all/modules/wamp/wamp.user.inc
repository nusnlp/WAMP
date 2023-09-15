<?php
// @author Chan Hong Guan chanhg -at- comp.nus.edu.sg
// $Id: wamp.user.inc 165 2009-08-17 11:34:46Z chanhg $

/**
 * @defgroup forms Forms
 * @{
 * @}
 */

/**
 * @ingroup wamp
 * @{
 */

define(ESSAY_NOT_STARTED, 0);
define(ESSAY_NOT_STARTED_STR, "NOT STARTED");
define(ESSAY_IN_PROGRESS, 1);
define(ESSAY_IN_PROGRESS_STR, "IN PROGRESS");
define(ESSAY_SUBMITTED, 2);
define(ESSAY_SUBMITTED_STR, "SUBMITTED");
define(ESSAY_ANNOTATION_IN_PROGRESS, 3);
define(ESSAY_ANNOTATION_COMPLETED, 4);

/**
 * Pulls in the module listing page for an annotator and
 * redirects all other users to the homepage.
 *
 * @see wamp_overview_form_module_form()
 */
function wamp_view_essays_form($record = array())
{
    $type = get_user_type();

    //override breadcrumb
    $links[] = l(t('Home'),'');
    //The section below is commented out to remove module information.
    // $links[] = l(t('Modules'), 'wamp');
    $links[] = l(t('View Essays'),'wamp/view_essays');
    drupal_set_breadcrumb($links);

    if ($type == 'annotator') {
        $title = drupal_get_title();
        drupal_set_title($title);
        $output .= drupal_get_form('wamp_view_essays_form_module_form', $type);
    }
    else {
        drupal_set_message('You have been redirected to the homepage as the
                link you have clicked on is for annotators only');
        drupal_goto('node');
    }

    return $output;
}

/**
 * Build a form to display the submitted essays in an assignment prompt.
 *
 * Loads all records and builds an listing with links for annotating or viewing
 * the annotations
 *
 * @ingroup forms
 * @see theme_wamp_overview_form_all_essays_form()
 * @see _wamp_overview_form_all_essays_links()
 */
function wamp_view_essays_form_module_form(&$form_state, $type)
{

    //bring the $user into context
    global $user;

    if ($type == 'annotator') {
        $header = array(
                array('data' => t('No.')),
                array('data' => t('Essay ID')),
                
                //The section below is added to display essay title.
                array('data' => t('Essay Title')),

                //The section below is commented out to remove student information.
                // array('data' => t('Matric Number')),

                array('data' => t('Status')),
                array('data' => t('Annotated')),
                t('Operations')
                );
    }

    if ($type == 'annotator') {
        //The section below is commented out to display the essay title.
        //
        // $sql = "SELECT *  
        //     FROM {wamp_essay_revision}
        // WHERE status=2";
        // $query_count = "SELECT COUNT(nid)
        //     FROM {wamp_essay_revision}
        // WHERE status=2";
        $sql = 'SELECT r.nid, r.vid, r.essay_id, r.bad_essay, r.needs_editing, nr.title, nr.body
        FROM {wamp_essay_revision} r, {node_revisions} nr
        WHERE r.nid = nr.nid
			AND r.status=2';
        $query_count = 'SELECT COUNT(r.nid)
        FROM {wamp_essay_revision} r, {node_revisions} nr
        WHERE r.nid = nr.nid
			AND r.status=2';

        $sql .= " ORDER BY nid ASC";

        /**
         * The section below is commented out to remove check for tutors & spectators.
         * 
        $sql = "SELECT *  
            FROM {wamp_essay_revision} r, {wamp_module} m, {wamp_essay_template} t,
        {wamp_module_group} mg, {wamp_module_group_student} mgs
        WHERE m.module_id = t.module_id AND r.essaytemplate_id = t.essaytemplate_id
            AND m.module_id = mg.module_id AND mg.display_flag = 1
            AND mgs.student_id = r.student_id AND mg.modulegroup_id = mgs.modulegroup_id
			AND r.status=2";
        $query_count = "SELECT COUNT(r.nid)
            FROM {wamp_essay_revision} r, {wamp_module} m, {wamp_essay_template} t,
        {wamp_module_group} mg, {wamp_module_group_student} mgs
        WHERE m.module_id = t.module_id AND r.essaytemplate_id = t.essaytemplate_id
            AND m.module_id = mg.module_id AND mg.display_flag = 1
            AND mgs.student_id = r.student_id AND mg.modulegroup_id = mgs.modulegroup_id
            AND r.status=2";
        $count_spectator = db_result(
                db_query('SELECT COUNT(m.module_id) FROM {wamp_module} m, {wamp_module_spectator} ms
                    WHERE m.module_id = ms.module_id AND spectator_id=%d', array($user->uid)));
        if ($count_spectator > 0) {

            //checks that the user is spectating the module
            $sql .= " AND m.module_id IN (

                SELECT m.module_id FROM {wamp_module} m, {wamp_module_spectator} ms
                WHERE m.module_id = ms.module_id AND ms.spectator_id = $user->uid
                )";
            $query_count .= " AND m.module_id IN (

                SELECT m.module_id FROM {wamp_module} m, {wamp_module_spectator} ms
                WHERE m.module_id = ms.module_id AND ms.spectator_id = $user->uid
                )";
        }
        else {
            //checks that the user is tutoring the module
            $sql .= " AND mg.modulegroup_id IN (

                SELECT mg.modulegroup_id
                FROM {wamp_module} m, {wamp_essay_template} t,
                {wamp_module_group} mg, {wamp_module_group_tutor} mgt
            WHERE m.module_id = mg.module_id AND mg.modulegroup_id = mgt.modulegroup_id
                AND t.module_id = m.module_id
                AND mgt.tutor_id=$user->uid
                )";

            $query_count .= " AND mg.modulegroup_id IN (

                SELECT mg.modulegroup_id
                FROM {wamp_module} m, {wamp_essay_template} t,
                {wamp_module_group} mg, {wamp_module_group_tutor} mgt
            WHERE m.module_id = mg.module_id AND mg.modulegroup_id = mgt.modulegroup_id
                AND t.module_id = m.module_id
                AND mgt.tutor_id=$user->uid
                )";
        }
//AND mg.modulegroup_id = ".$record['modulegroup_id']."
        //$sql .= " ORDER BY r.essay_id ASC";
        $sql .= " ORDER BY r.nid ASC";
		//echo $sql;
        //$sql .= tablesort_sql($header);
        */
    }
    //if (!empty($record->essaytemplate_id))
    $result = pager_query($sql, 50, 0, $query_count);
    $destination = drupal_get_destination();

    $essays = array();
    $revision_number = 1;
    $num = 1;
    $last = -1;
    while ($essayrevision = db_fetch_object($result))
    {
        $essays[$essayrevision->vid] = '';
        if ($type == 'annotator') {
            $form['num'][$essayrevision->vid] = array('#value' => $num);
            $form['essay_num'][$essayrevision->vid] = array('#value' => $essayrevision->nid);
            //The section below is commented out to remove student information.
            // $form['matric'][$essayrevision->vid] = array('#value' => get_student_matric_num($essayrevision->student_id));

            //The section below is added to display essay title.
            $form['essay_title'][$essayrevision->vid] = array('#value' => $essayrevision->title);

            //check if there is any prior annotation
            $sql_find = "SELECT nid from {wamp_essay_annotation} where nid=%d and vid=%d and annotator_id=%d";
            $nid = db_result(db_query($sql_find, array($essayrevision->nid, $essayrevision->vid, $user->uid)));
            $form['annotated'][$essayrevision->vid] = array('#value' => t('N'));
            if (!empty($nid)) $form['annotated'][$essayrevision->vid] = array('#value' => t('Y'));
        }
        /**
         * The section below is commented out to remove check for tutors & spectators.
         * 
        //if ($essaysrevision->nid == $last) continue;
        //$last = $essaysrevision->nid;
        $essays[$essayrevision->vid.".".$essayrevision->modulegroup_id] = '';
        if ($type == 'annotator') {
            $form['num'][$essayrevision->vid.".".$essayrevision->modulegroup_id] = array('#value' => $num);
            //$form['essay_num'][$essayrevision->vid.".".$essayrevision->modulegroup_id] = array('#value' => $essayrevision->essay_id);
            $form['essay_num'][$essayrevision->vid.".".$essayrevision->modulegroup_id] = array('#value' => $essayrevision->nid);
            $form['matric'][$essayrevision->vid.".".$essayrevision->modulegroup_id] = array('#value' => get_student_matric_num($essayrevision->student_id));

            //check if there is any prior annotation
            $sql_find = "SELECT nid from {wamp_essay_annotation} where nid=%d and vid=%d and annotator_id=%d";
            $nid = db_result(db_query($sql_find, array($essayrevision->nid, $essayrevision->vid, $user->uid)));
            $form['annotated'][$essayrevision->vid.".".$essayrevision->modulegroup_id] = array('#value' => t('N'));
            if (!empty($nid)) $form['annotated'][$essayrevision->vid.".".$essayrevision->modulegroup_id] = array('#value' => t('Y'));
        }
        */

        if ($essayrevision->status == ESSAY_IN_PROGRESS)
            $essay_status = ESSAY_IN_PROGRESS_STR;
        else if ($essayrevision->status == ESSAY_SUBMITTED) {
            $essay_status = ESSAY_SUBMITTED_STR;
        }



        // $form['operations'][$essayrevision->vid.".".$essayrevision->modulegroup_id] = array(
        $form['operations'][$essayrevision->vid] = array(
                '#type' => 'markup',
                '#value' => _wamp_overview_form_all_essays_links($essayrevision),
                );

        $form['pager'] = array('#value' => theme('pager', NULL, 50, 0));

        $revision_number++;
        $num++;
    }
    return $form;
}

/**
 * Theme wamp module overview form
 *
 * @ingroup themeable
 * @see wamp_module_overview_form()
 */
function theme_wamp_view_essays_form_module_form($form)
{

    // Overview table:
    $type = get_user_type();
    if ($type == 'annotator') {
        $header = array(
                array('data' => t('No.')),
                array('data' => t('Essay ID')),

                //The section below is added to display essay title.
                array('data' => t('Essay Title')),

                //The section below is commented out to remove student information.
                // array('data' => t('Matric Number')),
                array('data' => t('Annotated')),
                t('Operations')
                );
    }


    if (isset($form['operations']) && is_array($form['operations']))
    {
        foreach (element_children($form['operations']) as $key)
        {
            if ($type == 'annotator') {
                $rows[] = array(
                        drupal_render($form['num'][$key]),
                        drupal_render($form['essay_num'][$key]),

                        //The section below is added to display essay title.
                        drupal_render($form['essay_title'][$key]),

                        //The section below is commented out to remove student information.
                        // drupal_render($form['matric'][$key]),

                        drupal_render($form['annotated'][$key]),
                        drupal_render($form['operations'][$key]),
                        );
            }
        }
    }
    else
    {
        $rows[] = array(array('data' => t('No essay submissions available.'), 'colspan' => '4'));
    }

    $output .= theme('table', $header, $rows);
    if ($form['pager']['#value'])
    {
        $output .= drupal_render($form['pager']);
    }

    $output .= drupal_render($form);

    return $output;

}


//The section below is commented out to remove the module listing form.
//
// /**
//  * Pulls in the module listing page for an annotator and
//  * redirects all other users to the homepage.
//  *
//  * @see wamp_overview_form_module_form()
//  */
// function wamp_overview_form($record = array())
// {
//     $type = get_user_type();

//     //override breadcrumb
//     $links[] = l(t('Home'),'');
//     $links[] = l(t('Modules'),'wamp');
//     drupal_set_breadcrumb($links);

//     if ($type == 'annotator') {

//         $title = drupal_get_title();
//         $title .= ' you are currently teaching';
//         drupal_set_title($title);
//         $output .= drupal_get_form('wamp_overview_form_module_form', $type);
//     }
//     else {
//         drupal_set_message('You have been redirected to the homepage as the
//                 link you have clicked on is for annotators only');
//         drupal_goto('node');
//     }

//     return $output;
// }

//The section below is commented out to remove the module overview form.
//
// /**
//  * Builds a sortable overview of the list of modules an annotator is annotating
//  * or spectator is spectating
//  *
//  * @ingroup forms
//  * @see theme_wamp_overview_form_module_form()
//  * @see _wamp_overview_module_form_title_links()
//  */
// function wamp_overview_form_module_form(&$form_state, $type = '')
// {
//     global $user;
//     if ($type == '') $type = get_user_type();

//     //override breadcrumb
//     $header = array(
//             array('data' => t('Module Code'), 'field' => 'm.module_code'),
//             array('data' => t('Module Title'), 'field' => 'm.module_title'),
//             array('data' => t('Group #'), 'field' => 'mg.group_num'),
//             array('data' => t('Group Name'), 'field' => 'mg.group_name'),
//             t('Operations')
//             );

//     if ($type == 'annotator') {
//         $sql = 'SELECT mg.modulegroup_id, m.module_code, m.module_title,
//             mg.group_num, mg.group_name, m.module_id
//                 FROM {wamp_module} m, {wamp_module_group} mg, {wamp_module_group_tutor} mgt
//                 WHERE m.module_id = mg.module_id AND mg.modulegroup_id = mgt.modulegroup_id
//                 AND mgt.tutor_id = %d AND m.display_flag = 1';

//         $count_spectator = db_result(
//                 db_query('SELECT COUNT(m.module_id) FROM {wamp_module} m, {wamp_module_spectator} ms
//                     WHERE m.module_id = ms.module_id AND spectator_id=%d', array($user->uid)));
//         if ($count_spectator > 0) {
//             drupal_set_title('Modules you are currently spectating');
//             $sql = 'SELECT mg.modulegroup_id, m.module_code, m.module_title,
//                 mg.group_num, mg.group_name, m.module_id
//                     FROM {wamp_module} m, {wamp_module_group} mg, {wamp_module_group_tutor} mgt
//                     WHERE m.module_id = mg.module_id AND mg.modulegroup_id = mgt.modulegroup_id
//                     AND m.display_flag = 1
//                     AND m.module_id IN (

//                             SELECT m.module_id FROM {wamp_module} m, {wamp_module_spectator} ms
//                             WHERE m.module_id = ms.module_id AND ms.spectator_id = %d
//                             )';
//         }
//     }
//     $sql .= tablesort_sql($header);

//     $result = db_query($sql, array($user->uid));
//     $destination = drupal_get_destination();

//     $modules = array();
//     while ($module = db_fetch_object($result))
//     {

//         $modules[$module->modulegroup_id] = '';
//         $form['module_code'][$module->modulegroup_id] = array(
//                 '#type' => 'markup',
//                 '#value' => _wamp_overview_module_form_title_links($module, $module->module_code)
//                 );
//         $form['module_title'][$module->modulegroup_id] = array(
//                 '#type' => 'markup',
//                 '#value' => _wamp_overview_module_form_title_links($module, $module->module_title)
//                 );
//         $form['group_num'][$module->modulegroup_id] = array(
//                 '#type' => 'markup',
//                 '#value' => _wamp_overview_module_form_title_links($module, $module->group_num)
//                 );
//         $form['group_name'][$module->modulegroup_id] = array('#value' => $module->group_name);


//         $form['operations'][$module->modulegroup_id] = array(
//                 '#type' => 'markup',
//                 //		    '#value' => _wamp_module_record_links($module),
//                 );
//     }

//     return $form;
// }


//The section below is commented out to remove the module listing form.
//
// /**
//  * Build the links for the module listing form.
//  *
//  * @see wamp_overview_form_module_form()
//  */
// function _wamp_overview_module_form_title_links($record, $label)
// {
//     $links['edit'] = array(
//             'title' => t($label),
//             'href' => "wamp/prompts/".$record->module_id.'.'.$record->modulegroup_id,
//             'alt' => t('Show essays'),
//             'html' => TRUE,
//             );

//     $output = theme('links', $links);

//     $final = "<span>$output</span><span class=\"hovertip\">Click essay title to show submissions</span>";

//     return $output;
// }

/**
 * @defgroup themeable Themable
 * @{
 * @}
 */

//The section below is commented out to remove the themable module overview form.
//
// /**
//  * Theme wamp module overview form
//  *
//  * @ingroup themeable
//  * @see wamp_module_overview_form()
//  */
// function theme_wamp_overview_form_module_form($form)
// {
//     // Overview table:
//     $header = array(
//             //    theme('table_select_header_cell'),
//             array('data' => t('Module Code'), 'field' => 'm.module_code'),
//             array('data' => t('Module Title'), 'field' => 'm.module_title'),
//             array('data' => t('Group #'), 'width' => 60, 'align' => 'center', 'field' => 'mg.group_num'),
//             array('data' => t('Group Name'), 'field' => 'mg.group_name'),
//             t('Operations')
//             );


//     if (isset($form['operations']) && is_array($form['operations']))
//     {
//         foreach (element_children($form['operations']) as $key)
//         {
//             $rows[] = array(

//                     drupal_render($form['module_code'][$key]),
//                     drupal_render($form['module_title'][$key]),
//                     drupal_render($form['group_num'][$key]),
//                     drupal_render($form['group_name'][$key]),
//                     drupal_render($form['operations'][$key]),
//                     );
//         }
//     }
//     else
//     {
//         $rows[] = array(array('data' => t('No modules available.'), 'colspan' => '5'));
//     }

//     $output .= theme('table', $header, $rows);
//     if ($form['pager']['#value'])
//     {
//         $output .= drupal_render($form['pager']);
//     }

//     $output .= drupal_render($form);

//     return $output;
// }

//The section below is commented out to remove the essay listing page for selected assignment prompt.
//
// /**
//  * Pulls in the essay listing page for an assignment prompt.
//  *
//  * @see wamp_overview_form_all_essays_form()
//  */
// function wamp_overview_form_all_essays($record, $type = '') {

//     if ($type == '') $type = get_user_type();
//     $links[] = l(t('Home'),'');
//     $links[] = l(t('Modules'),'wamp');
//     $links[] = l(t('Assignments'),'wamp/prompts/'.$record['module_id'].'.'.$record['modulegroup_id']);
//     $links[] = l(t('Submissions'),'wamp/prompts/essays/'.$record['essaytemplate_id'].'.'.$record['modulegroup_id']);
//     drupal_set_breadcrumb($links);

//     if (!empty($record['essaytemplate_id'])) {
//         if ($type == 'annotator') {
//             $title = drupal_get_title();
//             $title .= ' for '.$record['title'];
//             drupal_set_title($title);
//         }
//         $output .= drupal_get_form('wamp_overview_form_all_essays_form', $record, $type);
//     }

//     return $output;
// }

//The section below is commented out to remove the essay listing page form for selected assignment prompt.
//
// /**
//  * Build a form to display the submitted essays in an assignment prompt.
//  *
//  * Loads all records and builds an listing with links for annotating or viewing
//  * the annotations
//  *
//  * @ingroup forms
//  * @see theme_wamp_overview_form_all_essays_form()
//  * @see _wamp_overview_form_all_essays_links()
//  */
// function wamp_overview_form_all_essays_form(&$form_state, $record, $type)
// {

//     //bring the $user into context
//     global $user;

//     $essaytemplate_id = $record['essaytemplate_id'];
//     if ($type == 'annotator') {
//         $header = array(
//                 array('data' => t('No.')),
//                 array('data' => t('Essay ID')),
//                 array('data' => t('Matric Number')),
//                 array('data' => t('Status')),
//                 array('data' => t('Annotated')),
//                 t('Operations')
//                 );
//     }

//     if ($type == 'annotator') {
//         $sql = "SELECT *  
//             FROM {wamp_essay_revision} r, {wamp_module} m, {wamp_essay_template} t,
//         {wamp_module_group} mg, {wamp_module_group_student} mgs
//         WHERE m.module_id = t.module_id AND r.essaytemplate_id = t.essaytemplate_id
//             AND m.module_id = mg.module_id AND mg.modulegroup_id = mgs.modulegroup_id
//             AND mgs.student_id = r.student_id
//             AND mgs.modulegroup_id = ".$record['modulegroup_id']."
//             AND r.status=2 AND r.essaytemplate_id = " . $record['essaytemplate_id'];
//         $query_count = "SELECT COUNT(r.nid)
//             FROM {wamp_essay_revision} r, {wamp_module} m, {wamp_essay_template} t,
//         {wamp_module_group} mg, {wamp_module_group_student} mgs
//         WHERE m.module_id = t.module_id AND r.essaytemplate_id = t.essaytemplate_id
//             AND m.module_id = mg.module_id AND mg.modulegroup_id = mgs.modulegroup_id
//             AND mgs.student_id = r.student_id
//             AND mgs.modulegroup_id = ".$record['modulegroup_id']."
//             AND r.status=2 AND r.essaytemplate_id = " . $record['essaytemplate_id'];
//         //spectators
//         $count_spectator = db_result(
//                 db_query('SELECT COUNT(m.module_id) FROM {wamp_module} m, {wamp_module_spectator} ms
//                     WHERE m.module_id = ms.module_id AND spectator_id=%d', array($user->uid)));
//         if ($count_spectator > 0) {

//             //checks that the user is spectating the module
//             $sql .= " AND m.module_id IN (

//                 SELECT m.module_id FROM {wamp_module} m, {wamp_module_spectator} ms
//                 WHERE m.module_id = ms.module_id AND ms.spectator_id = $user->uid
//                 )";
//             $query_count .= " AND m.module_id IN (

//                 SELECT m.module_id FROM {wamp_module} m, {wamp_module_spectator} ms
//                 WHERE m.module_id = ms.module_id AND ms.spectator_id = $user->uid
//                 )";
//         }
//         else {

//             //checks that the user is tutoring the module
//             $sql .= " AND mg.modulegroup_id IN (

//                 SELECT mg.modulegroup_id
//                 FROM {wamp_module} m, {wamp_essay_template} t,
//                 {wamp_module_group} mg, {wamp_module_group_tutor} mgt
//             WHERE m.module_id = mg.module_id AND mg.modulegroup_id = mgt.modulegroup_id
//                 AND t.module_id = m.module_id
//                 AND mg.modulegroup_id = ".$record['modulegroup_id']." AND mgt.tutor_id=$user->uid
//                 )";

//             $query_count .= " AND mg.modulegroup_id IN (

//                 SELECT mg.modulegroup_id
//                 FROM {wamp_module} m, {wamp_essay_template} t,
//                 {wamp_module_group} mg, {wamp_module_group_tutor} mgt
//             WHERE m.module_id = mg.module_id AND mg.modulegroup_id = mgt.modulegroup_id
//                 AND t.module_id = m.module_id
//                 AND mg.modulegroup_id = ".$record['modulegroup_id']." AND mgt.tutor_id=$user->uid
//                 )";
//         }

//         $sql .= " ORDER BY r.essay_id ASC";
//         $sql .= tablesort_sql($header);
//     }
//     if (!empty($record->essaytemplate_id))
//         $result = pager_query($sql, 50, 0, $query_count);
//     $destination = drupal_get_destination();

//     $essays = array();
//     $revision_number = 1;
//     $num = 1;
//     while ($essayrevision = db_fetch_object($result))
//     {
//         $essays[$essayrevision->vid] = '';

//         if ($type == 'annotator') {
//             $form['num'][$essayrevision->vid] = array('#value' => $num);
//             $form['essay_num'][$essayrevision->vid] = array('#value' => $essayrevision->essay_id);
//             $form['matric'][$essayrevision->vid] = array('#value' => get_student_matric_num($essayrevision->student_id));

//             //check if there is any prior annotation
//             $sql_find = "SELECT nid from {wamp_essay_annotation} where nid=%d and vid=%d and annotator_id=%d";
//             $nid = db_result(db_query($sql_find, array($essayrevision->nid, $essayrevision->vid, $user->uid)));
//             $form['annotated'][$essayrevision->vid] = array('#value' => t('N'));
//             if (!empty($nid)) $form['annotated'][$essayrevision->vid] = array('#value' => t('Y'));
//         }

//         if ($essayrevision->status == ESSAY_IN_PROGRESS)
//             $essay_status = ESSAY_IN_PROGRESS_STR;
//         else if ($essayrevision->status == ESSAY_SUBMITTED) {
//             $essay_status = ESSAY_SUBMITTED_STR;
//         }



//         $form['operations'][$essayrevision->vid] = array(
//                 '#type' => 'markup',
//                 '#value' => _wamp_overview_form_all_essays_links($essayrevision),
//                 );

//         $form['pager'] = array('#value' => theme('pager', NULL, 50, 0));

//         $revision_number++;
//         $num++;
//     }

//     return $form;
// }

//The section below is commented out to remove the annotate link for selected assignment prompt.
//
/**
 * Build the annotate link for annotators and view annotation link for spectators
 * for a given assignment prompt.
 *
 * @see wamp_overview_form_all_essays_form()
 */
function _wamp_overview_form_all_essays_links($record)
{
    global $user;

    if ($record->status == ESSAY_IN_PROGRESS)
    {
        $links['edit'] = array(
                'title' => t('Update Essay'),
                'href' => "wamp/wamp_essay/".$record->nid.".".$record->vid."/update_essay",
                'html' => TRUE,
                );
    }

    if (wamp_essay_access('annotate', NULL))
    {
        $links['edit'] = array(
            'title' => t('Annotate'),
            'href' => "wamp/wamp_essay/".$record->nid.".".$record->vid."/annotate_essay",
            'html' => TRUE,
        );
        /**
         * The section below is commented out to remove check for tutors & spectators.
         * 
        $spectator = is_spectator($record->module_id);
        if ($spectator == false) {

            $links['edit'] = array(
                    'title' => t('Annotate'),
                    'href' => "wamp/wamp_essay/".$record->nid.".".$record->vid."/annotate_essay",
                    'html' => TRUE,
                    );
        }
        else if ($spectator == true) {

            //readonly links to other annotator's annotations
            $result = db_query("SELECT annotator_id FROM {wamp_essay_annotation} a
                    WHERE a.nid=%d AND a.vid=%d AND a.annotator_id <> %d AND a.annotator_id <> 1",
                    array($record->nid, $record->vid, $user->uid)
                    );
            while ($readonly = db_fetch_object($result)) {
                $name = strtok(get_display_name($readonly->annotator_id), ' ');
                $links['ro_'.$readonly->annotator_id] = array(
                        'title' => t($name.'\'s'),
                        'href' => 'wamp/wamp_essay/'.$record->nid.'.'.$record->vid.'.'.$record->modulegroup_id.'.'.$readonly->annotator_id.'/view_annotation',
                        'html' => TRUE,
                        );
            }
        }
         */
    }
    return theme('links', $links);
}

//The section below is commented out to remove the themable essay listing page form for selected assignment prompt.
//
// /**
//  * Theme essay listing form
//  *
//  * @ingroup themeable
//  */
// function theme_wamp_overview_form_all_essays_form($form)
// {

//     // Overview table:
//     $type = get_user_type();
//     if ($type == 'annotator') {
//         $header = array(
//                 array('data' => t('No.')),
//                 array('data' => t('Essay ID')),
//                 array('data' => t('Matric Number')),
//                 array('data' => t('Annotated')),
//                 t('Operations')
//                 );
//     }


//     if (isset($form['operations']) && is_array($form['operations']))
//     {
//         foreach (element_children($form['operations']) as $key)
//         {
//             if ($type == 'annotator') {
//                 $rows[] = array(
//                         drupal_render($form['num'][$key]),
//                         drupal_render($form['essay_num'][$key]),
//                         drupal_render($form['matric'][$key]),
//                         drupal_render($form['annotated'][$key]),
//                         drupal_render($form['operations'][$key]),
//                         );
//             }
//         }
//     }
//     else
//     {
//         $rows[] = array(array('data' => t('No essay submissions available.'), 'colspan' => '4'));
//     }

//     $output .= theme('table', $header, $rows);
//     if ($form['pager']['#value'])
//     {
//         $output .= drupal_render($form['pager']);
//     }

//     $output .= drupal_render($form);

//     return $output;
// }

//The section below is commented out to remove the assignment prompt form for the selected module.
//
// /**
//  * Pulls in the assignment listing page for a module.
//  *
//  * @see wamp_overview_form_essay_reqs_form()
//  */
// function wamp_overview_form_essay_reqs($type = '', $record = array()) {

//     if ($type == '') $type = get_user_type();
//     $links[] = l(t('Home'),'');
//     $links[] = l(t('Modules'),'wamp');
//     $links[] = l(t('Assignments'),'wamp/prompts/'.$record['module_id'].'.'.$record['modulegroup_id']);
//     drupal_set_breadcrumb($links);

//     if (!empty($record->modulegroup_id)) {
//         if ($type == 'annotator') {
//             $title = drupal_get_title();
//             $title .= ' for Students in Module ';
//             drupal_set_title($title .
//                     db_result(db_query('SELECT module_code FROM {wamp_module} WHERE module_id=%d', array($record['module_id']))).' '.
//                     db_result(db_query('SELECT group_name FROM {wamp_module_group} WHERE modulegroup_id=%d', array($record['modulegroup_id'])))
//                     );
//         }
//         $output .= drupal_get_form('wamp_overview_form_essay_reqs_form', $type, $record);
//     }

//     return $output;
// }

//The section below is commented out to remove the assignment prompt form for the selected module.
//
// /**
//  * Build a form to display all the assignment prompts for a given module and group.
//  *
//  * Loads all records and builds an overview form with individual assignment prompts
//  *
//  * @ingroup forms
//  * @see theme_wamp_overview_form_essay_reqs_form()
//  * @see _wamp_overview_form_essay_reqs_title_links()
//  */
// function wamp_overview_form_essay_reqs_form(&$form_state, $type = '', $record = array())
// {

//     //bring the $user into context
//     global $user;

//     $header = array(
//             array('data' => t('Essay Title'), 'field' => 't.title'),
//             array('data' => t('Module')),
//             array('data' => t('Status')),
//             t('Operations')
//             );

//     if ($type == 'annotator') {
//         $sql = "SELECT t.title, m.module_id, m.module_code, t.essaytemplate_id, mg.modulegroup_id
//             FROM {wamp_module} m, {wamp_essay_template} t,
//         {wamp_module_group} mg, {wamp_module_group_tutor} mgt
//         WHERE m.module_id = mg.module_id AND mg.modulegroup_id = mgt.modulegroup_id
//             AND t.module_id = m.module_id
//             AND mg.modulegroup_id = ".$record['modulegroup_id']." AND mgt.tutor_id=$user->uid";
//         $sql .= tablesort_sql($header);
//         $query_count = "SELECT COUNT(t.essaytemplate_id)
//             FROM {wamp_module} m, {wamp_essay_template} t,
//         {wamp_module_group} mg, {wamp_module_group_tutor} mgt
//         WHERE m.module_id = mg.module_id AND mg.modulegroup_id = mgt.modulegroup_id
//             AND t.module_id = m.module_id
//             AND mg.modulegroup_id = ".$record['modulegroup_id']." AND mgt.tutor_id=$user->uid";

//         //spectators
//         $count_spectator = db_result(
//                 db_query('SELECT COUNT(m.module_id) FROM {wamp_module} m, {wamp_module_spectator} ms
//                     WHERE m.module_id = ms.module_id AND spectator_id=%d', array($user->uid)));
//         if ($count_spectator > 0) {


//             $sql = "SELECT t.title, m.module_id, m.module_code, t.essaytemplate_id, mg.modulegroup_id
//                 FROM {wamp_module} m, {wamp_essay_template} t,
//             {wamp_module_group} mg, {wamp_module_group_tutor} mgt
//             WHERE m.module_id = mg.module_id AND mg.modulegroup_id = mgt.modulegroup_id
//                 AND t.module_id = m.module_id
//                 AND mg.modulegroup_id = ".$record['modulegroup_id']."
//                 AND m.module_id IN (

//                         SELECT m.module_id FROM {wamp_module} m, {wamp_module_spectator} ms
//                         WHERE m.module_id = ms.module_id AND ms.spectator_id = $user->uid
//                         )";

//             $sql .= tablesort_sql($header);
//             $query_count = "SELECT COUNT(t.essaytemplate_id)
//                 FROM {wamp_module} m, {wamp_essay_template} t,
//             {wamp_module_group} mg, {wamp_module_group_tutor} mgt
//             WHERE m.module_id = mg.module_id AND mg.modulegroup_id = mgt.modulegroup_id
//                 AND t.module_id = m.module_id
//                 AND mg.modulegroup_id = ".$record['modulegroup_id']."
//                 AND m.module_id IN (

//                         SELECT m.module_id FROM {wamp_module} m, {wamp_module_spectator} ms
//                         WHERE m.module_id = ms.module_id AND ms.spectator_id = $user->uid
//                         )";
//         }
//     }
//     if (!empty($record['modulegroup_id']))
//         $result = pager_query($sql, 50, 0, $query_count);
//     $destination = drupal_get_destination();


//     $essays = array();
//     $i=0;
//     while ($essayrevision = db_fetch_object($result))
//     {
//         $essays[$essayrevision->essaytemplate_id] = '';
//         $form['essay_title'][$essayrevision->essaytemplate_id] = array(
//                 '#type' => 'markup',
//                 '#value' => _wamp_overview_form_essay_reqs_title_links($essayrevision)
//                 );
//         $form['module_code'][$essayrevision->essaytemplate_id] = array('#value' => $essayrevision->module_code);

//         $form['operations'][$essayrevision->essaytemplate_id] = array(
//                 '#type' => 'markup',
//                 //'#value' =>_wamp_overview_form_essay_reqs_op_links($essayrevision),
//                 );
//     }
//     return $form;




// }

//The section below is commented out to remove the assignment prompt link for the selected module.
//
// /**
//  * Build the links to the essays for an assignment prompt
//  *
//  * @see wamp_overview_form_essay_reqs_form()
//  */
// function _wamp_overview_form_essay_reqs_title_links($record)
// {
//     //$path = drupal_get_path('module', 'scaffolding_example') . '/images/';
//     if ($record->status == ESSAY_NOT_STARTED)
//     {
//         $links['edit'] = array(
//                 'title' => t($record->title),
//                 'href' => "wamp/prompts/essays/".$record->essaytemplate_id.'.'.$record->modulegroup_id,
//                 'alt' => t('Show essays'),
//                 'html' => TRUE,
//                 );
//     }

//     $output = theme('links', $links);

//     $final = "<span>$output</span><span class=\"hovertip\">Click essay title to show submissions</span>";

//     return $final;
// }

//The section below is commented out to remove the themable assignment form for the selected module.
//
// /**
//  * Theme wamp assignment prompt listing form
//  *
//  * @ingroup themeable
//  * @see wamp_overview_form_essay_reqs_form()
//  */
// function theme_wamp_overview_form_essay_reqs_form($form)
// {
//     // Overview table:
//     $header = array(
//             array('data' => t('Essay Title'), 'field' => 't.title'),
//             array('data' => t('Module')),
//             array('data' => t('Status')),
//             t('Operations')
//             );


//     if (isset($form['essay_title']) && is_array($form['essay_title']))
//     {
//         foreach (element_children($form['essay_title']) as $key)
//         {
//             $rows[] = array(

//                     drupal_render($form['essay_title'][$key]),
//                     drupal_render($form['module_code'][$key]),
//                     drupal_render($form['essay_status'][$key]),
//                     drupal_render($form['operations'][$key]),
//                     );
//         }
//     }
//     else
//     {
//         $rows[] = array(array('data' => t('No essay submissions available.'), 'colspan' => '4'));
//     }

//     $output .= theme('table', $header, $rows);
//     if ($form['pager']['#value'])
//     {
//         $output .= drupal_render($form['pager']);
//     }

//     $output .= drupal_render($form);

//     return $output;
// }

/**
 * @} End of "ingroup wamp".
 */

?>
