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

  $form['submit'] = ['#type' => 'submit', '#value' => 'Submit'];
  return $form;
}

/**
 * Implements hook_validate().
 */
function tripal_eutils_admin_settings_form_validate($form, &$form_state) {

  // TODO We would validate the key here.  Perhaps make a test query?

  // Prepare the URL and parameters (with API key)
  $url = 'https://eutils.ncbi.nlm.nih.gov/entrez/eutils/einfo.fcgi';
  $url .= '?api_key='.$form_state['values']['api_key'];

  // Make the request
  $response = drupal_http_request($url);

  // Error handling
  if (array_key_exists('error',$response))
  {
    if(preg_match('/\b(API key invalid)\b/',$response->data))
    {
      //drupal_set_message("Invalid API Key",'error');
      form_set_error('api_key',t("Invalid API Key"));
    }
    else if(preg_match('/\b(API key not wellformed)\b/',$response->data))
    {
      //drupal_set_message("Unknown or malformed API Key",'error');
      form_set_error('api_key',t("Unknown or malformed API Key"));
    }
    else
    {
      drupal_set_message("Other unknown error:".$response->data,'error');
      form_set_error('api_key',t("Unknown issue with API key"));
    }
  }
  // No apparent errors. Check that NCBI responded with the correct number of
  //   requests per second that API key users should have (currently 10)
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

  drupal_set_message("API Key validated and saved.");

}
