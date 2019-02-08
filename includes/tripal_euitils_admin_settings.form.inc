<?php

/**
 * @file
 * Admin form for the whole module.
 */

/**
 * Implements hook_form().
 */
function tripal_eutils_admin_settings_form($form, &$form_state) {

  $api_key = variable_get('tripal_eutils_ncbi_api_key');

  $form['preamble'] = ['#markup' => '<h3>Tripal EUtils</h3><p>This administrative page is for module-wide settings.  Please see the <a href="https://tripal-eutils.readthedocs.io/en/latest/">Online module documentation</a> for more information.</p>'];
  $form['api_key'] = [
    '#title' => t('NCBI API key'),
    '#type' => 'textfield',
    '#default_value' => $api_key ?? '',
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => FALSE,
    '#description' => 'NCBI API key.  An API key can improve the reliability of this module by allowing more requests/second to the NCBI servers.',
  ];

  $form['submit'] = ['#type' => 'submit', '#value' => 'submit'];
  return $form;
}

/**
 * Implements hook_validate().
 */
function tripal_eutils_admin_settings_form_validate($form, &$form_state) {

  // TODO We would validate the key here.  Perhaps make a test query?
}

/**
 * Implements hook_submit().
 */
function tripal_eutils_admin_settings_form_submit($form, &$form_state) {
  if (isset($form_state['values']['api_key'])) {
    $api_key = $form_state['values']['api_key'];
    variable_set('tripal_eutils_ncbi_api_key', $api_key);
  }
  else {
    variable_set('tripal_eutils_ncbi_api_key', NULL);
  }

  drupal_set_message("Form submitted.");

}
