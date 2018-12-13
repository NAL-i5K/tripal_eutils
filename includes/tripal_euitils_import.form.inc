<?php

/**
 * @file
 * Provides a simple admin form for checking and submitting creation requests.
 */

/**
 * Implements hook form.
 */
function tripal_eutils_import_form($form, &$form_state) {

  $form['instructions'] = [
    '#markup' => '<p>Please enter an accession and specify a database.</p>
<p>Press the <b>Preview Record</b> button to view the retrieved data and metadata.  Pressing <b>Create Chado Record</b> will create the record.</p>
',
  ];

  $db_choices = [
    'bioproject' => 'BioProject',
    'biosample' => 'Biosample',
    'assembly' => 'Assembly',
  ];

  $form['db'] = [
    '#type' => 'radios',
    '#title' => "NCBI Database",
    '#description' => "The database to query.",
    '#options' => $db_choices,
  ];

  $form['accession'] = [
    '#type' => 'textfield',
    '#title' => "NCBI Accession Number",
    '#description' => "Valid examples: (BioSample 744358 120060 2261463), (Assembly 557018, 91111, 751381), (BioProject 12384, 394253, 66853)",
  ];

  $form['callback'] = ['#type' => 'button', '#value' => "Preview Record"];

  if (isset($form_state['values']['parsed'])) {
    $form['data'] = [
      '#type' => 'fieldset',
      '#title' => 'Data',
    ];

    $form['data'][] = $form_state['values']['parsed'];

  }

  $form['options'] = ['#type' => 'fieldset', '#title' => "Options"];

  $form['options']['linked_records'] = [
    '#type' => 'checkbox',
    '#title' => "Create Linked Records",
    '#description' => "Each accession links to other genbank databases: you can create those Chado records as well.",
  ];

  $form['submit'] = ['#type' => 'submit', '#value' => "Create Chado Record"];

  return $form;
}

/**
 * Implements hook_form_validate().
 */
function tripal_eutils_import_form_validate($form, &$form_state) {

  $vals = $form_state['values'];

  $db = $vals['db'];
  $accession = $vals['accession'];

  if (!$db) {
    form_set_error('db', 'please select a valid db');
  }

  if (!$accession) {
    form_set_error('accession', 'please enter an accession');
  }
  $connection = new \EUtils();

  try {

    $connection->setPreview();

    $parsed = $connection->get($db, $accession);

    $form_state['values']['parsed'] = $parsed;

  }
  catch (\Exception $e) {

    tripal_set_message($e, TRIPAL_ERROR);
  }
}

/**
 * Implements hook_form_submit().
 */
function tripal_eutils_import_form_submit($form, &$form_state) {
  $vals = $form_state['values'];

  $db = $vals['db'];
  $accession = $vals['accession'];

  if (!$db) {
    form_set_error('db', 'please select a valid db');
  }

  if (!$accession) {
    form_set_error('accession', 'please enter an accession');
  }

  $connection = new \EUtils();

  try {
    $connection->get($db, $accession);
    tripal_set_message('Record Imported!', TRIPAL_NOTICE);

  }
  catch (\Exception $e) {
    tripal_set_message($e, TRIPAL_ERROR);
  }

}
