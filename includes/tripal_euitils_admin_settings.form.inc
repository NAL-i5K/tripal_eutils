<?php

/**
 * @file
 */

/**
 * Implements hook_menu().
 *
 * Admin form for the whole module.
 * - Configure whether to ignore missing attributes on import.
 */
function tripal_eutils_admin_settings_form($form, &$form_state) {

  $api_key = variable_get('tripal_eutils_ncbi_api_key');

  $form['api_key']= [
    '#title' => t('NCBI API key'),
    '#default_value' => $api_key ?? '',
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
  ];
  return $form;
}

/**
 * Implements hook_validate().
 */
function tripal_eutils_admin_settings_form_validate($form, &$form_state) {
  $api_key = $form_state['values']['api_key'];

  // Check API key in some way.

}

/**
 * Implements hook_submit().
 */
function tripal_eutils_admin_settings_form_submit($form, &$form_state) {
  $api_key = $form_state['values']['api_key'];

  variable_set('tripal_eutils_ncbi_api_key', $api_key);

}
