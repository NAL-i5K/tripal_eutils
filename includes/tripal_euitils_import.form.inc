<?php

/**
 * @file
 * Provides a simple admin form for checking and submitting creation requests.
 */

/**
 * Implements hook_form().
 */
function tripal_eutils_import_form($form, &$form_state) {

  $form['instructions'] = [
    '#markup' => t('<p>Please enter an accession and specify a database.</p>
                  <p>Press the <b>Preview Record</b> button to view the 
                  retrieved data and metadata.  Pressing <b>Create Chado 
                  Record</b> will create the record.</p>'),
  ];

  $db_choices = [
    'bioproject' => 'BioProject',
    'biosample' => 'Biosample',
    'assembly' => 'Assembly',
  ];

  $form['db'] = [
    '#type' => 'radios',
    '#title' => t('NCBI Database'),
    '#description' => t('The database to query.'),
    '#options' => $db_choices,
  ];

  $form['ignored_links'] = ['
  #type' => '#markup'];

  $form['accession'] = [
    '#type' => 'textfield',
    '#title' => t('NCBI Accession Number'),
    '#description' => t('Valid examples: (BioSample 744358 120060 SAMN02261463), (Assembly 91111, 751381, GCA_000516895.1), (BioProject 12384, 394253, 66853, PRJNA185471)'),
  ];

  $form['callback'] = [
    '#type' => 'button',
    '#value' => "Preview Record",
  ];

  if (isset($form_state['values']['parsed'])) {
    $form['data'] = [
      '#type' => 'fieldset',
      '#title' => 'Data',
    ];

    $form['data'][] = $form_state['values']['parsed'];

  }

  $form['options'] = [
    '#type' => 'fieldset',
    '#title' => "Options",
  ];

  $form['options']['linked_records'] = [
    '#type' => 'checkbox',
    '#title' => t('Create Linked Records'),
    '#description' => t('Each accession links to other NCBI databases: 
                          you can create those chado records as well.'),
    '#default_value' => 1,
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => "Create Chado Record",
  ];

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


  if (!$db or !$accession) {
    return;
  }

  $connection = new \EUtils();
  try {
    $connection->setPreview();
    $parsed = $connection->get($db, $accession);
    $form_state['values']['parsed'] = $parsed;
  }
  catch (\Exception $e) {
    tripal_set_message($e->getMessage(), TRIPAL_ERROR);
    return;
  }
}

/**
 * Implements hook_form_submit().
 */
function tripal_eutils_import_form_submit($form, &$form_state) {
  global $user;

  $vals = $form_state['values'];
  $db = $vals['db'];
  $accession = $vals['accession'];
  $create_linked_records = (bool) $vals['linked_records'];

  tripal_add_job("EUtils: Create Records for {$db}:{$accession}",
    'tripal_eutils', 'tripal_eutils_create_records', [
      $db,
      $accession,
      $create_linked_records,
    ], $user->uid);

  drupal_set_message('Job to create records have been submitted successfully.');
}
