<?php


/**
 * Implements hook_submit.
 *
 * This form lets users map attributes and other XML tags.
 */
function tripal_euitils_db_term_map_form($form, &$form_state, $db_type = NULL){

  if (!$db_type){
    tripal_set_message('what should we do here? nothing?', TRIPAL_ERROR);
    return false;
  }

  $form['instructions'] = ['#markup' => '<p>This area lets you configure what terms are used for each attribute.</p>'];

  $form['instructions']


}

function tripal_euitils_db_term_map_form_submit(){

}