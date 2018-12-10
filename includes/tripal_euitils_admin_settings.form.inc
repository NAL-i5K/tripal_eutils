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
  return $form;
}

/**
 * Implements hook_submit().
 */
function tripal_eutils_admin_settings_form_submit($form, &$form_state) {
  $form;
  $form_state;
  return;
}
