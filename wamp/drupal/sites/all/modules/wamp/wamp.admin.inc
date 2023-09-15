<?php
// @author Chan Hong Guan chanhg -at- comp.nus.edu.sg
// $Id: wamp.admin.inc 165 2009-08-17 11:34:46Z chanhg $

/**
 * @ingroup wamp
 * @{
 */

/**
 * Builds configuration form for WAMP. The configuration form has 1 setting,
 * which is the operating mode of the WAMP server. There are 2 options, namely
 * Testing Mode and Production Mode. In Testing Mode, firebug lite will be loaded
 * and javascript required for the system will be in verbose format. As for
 * production mode, no firebug lite will be loaded and minified javascript will
 * be used for maximum execution speed.
 * @see wamp_essay_annotate_form()
 * @see wamp_essay_view_annotation_form()
 * @see wamp_essay_view_corrected_essay_form()
 */
function wamp_settings_form(&$form_state) {

    $operating_mode_options = array(
        'testing' => 'Testing',
        'production' => 'Production'
    );

    $form['operating_mode'] = array(
        '#type' => 'radios',
        '#options' => $operating_mode_options,
        '#default_value' => variable_get('wamp_system', 'testing'),
        '#title' => t('Operating Mode'),
        '#required' => true,
    );

    $form['buttons']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
    );

    return $form;
}

/**
 * Submit handler for WAMP Settings form: Processes configuration of WAMP.
 * Sets the wamp_system variable.
 * 
 */
function wamp_settings_form_submit($form, &$form_state) {

    $values = $form_state['values'];
    variable_set('wamp_system', $values['operating_mode']);
    drupal_set_message('Operating Mode set successfully');
}

/**
 * @} End of "ingroup wamp".
 */
?>
