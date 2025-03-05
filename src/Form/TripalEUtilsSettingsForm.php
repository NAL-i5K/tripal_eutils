<?php

namespace Drupal\tripal_eutils\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * @file
 * Admin form for the whole tripal_eutils module.
 */


/**
 * Class TripalEUtilsSettingsForm.
 *
 * @package Drupal\tripal\Form
 *
 * @ingroup tripal
 */
class TripalEUtilsSettingsForm extends FormBase {

  /**
   * Returns a unique string identifying the form.
   *
   * @return string
   *   The unique string identifying the form.
   */
  public function getFormId() {
    return 'tripal_eutils_settings_form';
  }

  /**
   * Defines the settings form for Tripal EUtils.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   *
   * @return array
   *   Form definition array.
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $settings = \Drupal::config('tripal_eutils.settings');

    // NCBI API private key
    $ncbi_api_key = $form_state->getValue(
      'ncbi_api_key',
      $settings->get('tripal_eutils.ncbi_api_key')
    );

    $form['preamble'] = ['#markup' => '<h3>Tripal EUtils</h3><p>This administrative page is for module-wide settings. Please see the <a href="https://tripal-eutils.readthedocs.io/en/latest/">Online module documentation</a> for more information.</p>'];

    $form['ncbi_api_key'] = [
      '#title' => t('NCBI API key'),
      '#type' => 'textfield',
      '#default_value' => $ncbi_api_key ?? '',
      '#size' => 60,
      '#maxlength' => 128,
      '#required' => FALSE,
      '#description' => 'NCBI API key. An API key can improve the performance of this module by allowing more requests/second to the NCBI servers. For more information see <a href="https://support.nlm.nih.gov/kbArticle/?pn=KA-05316">What are API keys, and why might a user need them for NCBI services?</a>',
    ];

    $form['submit'] = [
      '#type' => 'submit',
      '#value' => t('Save'),
    ];

    return $form;
  }

  /**
   * Validate the form values.
   * The NCBI API key is validated by a request to NCBI.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $ncbi_api_key = $form_state->getValue('ncbi_api_key');
    if ($ncbi_api_key) {
      if (!preg_match("/^[a-z0-9]+$/", $ncbi_api_key)) {
        $form_state->setErrorByName('ncbi_api_key',
          t('Only lower case letters and numbers can be used in the API key.'));
      }
      else {
        // Perform a test query to NCBI to validate the key.
        // A valid API key will return XML, an invalid one will
        // throw an exception and return json.
        $url = 'https://eutils.ncbi.nlm.nih.gov/entrez/eutils/einfo.fcgi'
             . '?api_key=' . $ncbi_api_key;
        $body = '';
        $valid = FALSE;
        try {
          $response = \Drupal::httpClient()->get($url);
          $valid = TRUE;
        }
        catch (\Exception $e) {
          if (method_exists($e, 'hasResponse') and $e->hasResponse()) {
            $body = $e->getResponse()->getBody()->getContents();
          }
        }
        if (!$valid) {
          if ($body == '') {
            $form_state->setErrorByName('ncbi_api_key',
              t('No response received from NCBI'));
          }
          else {
            $values = json_decode($body, TRUE);
            if (!$values) {
              $form_state->setErrorByName('ncbi_api_key',
                t('Invalid json error response received from NCBI'));
            }
            elseif (array_key_exists('error', $values)) {
              $error = $values['error'];
              if (preg_match('/\b(API key invalid)\b/', $error)) {
                $form_state->setErrorByName('ncbi_api_key',
                  t('NCBI reports that the API key is not valid'));
              }
              elseif (preg_match('/\b(API key not wellformed)\b/', $error)) {
                $form_state->setErrorByName('ncbi_api_key',
                  t('NCBI reports that the API key is not well formed'));
              }
              else {
                $form_state->setErrorByName('ncbi_api_key',
                  t('NCBI reports an unexpected issue with the API key'));
              }
            }
          }
        }
      }
    }
  }

  /**
   * Form submission handler. Saves the form values to tripal settings.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $ncbi_api_key = $form_state->getValue('ncbi_api_key');
    \Drupal::configFactory()
      ->getEditable('tripal_eutils.settings')
      ->set('tripal_eutils.ncbi_api_key', $ncbi_api_key)
      ->save();

    $this->messenger()->addStatus('Settings have been saved.');
  }
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
