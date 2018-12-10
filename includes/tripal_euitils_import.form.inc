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
<p>Supported databases:  Biosample </p>

<p>Press the <b>Preview Record</b> button to view the retrieved data and metadata.  Pressing <b>Create Chado Record</b> will create the record.</p>
',
  ];

  $db_choices = [
    'project' => 'BioProject',
    'biosample' => 'Biosample',
    'assembly' => 'Assembly',
  ];

  $form['db'] = [
    '#type' => 'radios',
    '#title' => "NCBI Database",
    '#description' => "The database to query.  ONLY BIOSAMPLE FOR NOW",
    '#options' => $db_choices,
  ];

  $form['accession'] = [
    '#type' => 'textfield',
    '#title' => "NCBI Accession Number",
    '#description' => "Valid examples include: 744358 120060 2261463 2953603 4451765
     
     examples.  Note: if hte db is in the accession we... ignore the above selected DB?",
  ];

  $form['callback'] = ['#type' => 'button', '#value' => "Preview Record"];

  if (isset($form_state['values']['parsed'])) {

    $parsed = $form_state['values']['parsed'];

    $form['data'] = [
      '#type' => 'fieldset',
      '#title' => 'Data',
    ];

    foreach ($parsed as $key => $item) {

      if ($key == 'full_ncbi_xml') {
        continue;
      }

      if (is_string($item)) {

        $form['data'][] = [
          '#type' => 'item',
          '#title' => ucfirst($key),
          '#markup' => $item,
        ];
        continue;
      }

      if ($key === 'attributes') {
        $header = ['Key', 'Value'];

        $rows = [];

        foreach ($item as $record) {

          $rows[] = [$record['harmonized_name'], $record['value']];
        }

        $table = theme('table', ['rows' => $rows, 'header' => $header]);
        $form['data'][] = [
          '#type' => 'item',
          '#markup' => $table,
          '#title' => ucfirst($key),
        ];

        continue;
      }

      if ($key === 'accessions') {

        $header = ['DB', 'Value'];
        $rows = [];

        foreach ($item as $record) {

          $rows[] = [$record['db'], $record['value']];
        }

        $table = theme('table', ['rows' => $rows, 'header' => $header]);
        $form['data'][] = [
          '#markup' => $table,
          '#prefix' => ucfirst($key),
          '#type' => 'item',
        ];
        continue;

      }

      foreach ($item as $record) {

        if (is_string($record)) {

          $form['data'][] = [
            '#type' => 'item',
            '#title' => ucfirst($key),
            '#markup' => $record,
          ];
          continue;
        }

      }

    }
    // End submission state.
  }

  $form['options'] = ['#type' => 'fieldset', '#title' => "Options"];

  $form['options']['linked_records'] = [
    '#type' => 'checkbox',
    '#title' => "Create Linked Records",
    '#description' => "Each accession links to other genbank databases: you can create those chado records as well. ",
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

  // TODO: how do we know which parser and repo to use?  EUTils would handle that?
  try {
    $connection->get($db, $accession);

  }
  catch (\Exception $e) {

    tripal_set_message($e, TRIPAL_ERROR);
  }
}
