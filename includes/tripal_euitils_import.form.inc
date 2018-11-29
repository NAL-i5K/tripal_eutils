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
    '#markup' => '<p>Instructions for using this form.</p>
<p>Please enter an accession and specify a database.</p>
<p>Supported databases:  Biosample </p>',
  ];


  $form['linked_records']= ['#type' => 'radio', ''];

  $form['accession'] = [
    '#type' => 'textfield',
    '#title' => "NCBI Accession Number",
    '#description' => "Valid examples include:"
  ];

  $form['data'] = ['#type' => 'fieldset'];




  $form['submit'] = ['#type' => 'submit', '#value' => "Create Chado Record"];

  return $form;
}

function tripal_eutils_import_form_validate($form, &$form_state){

}

function tripal_eutils_import_form_submit($form, &$form_state){

}