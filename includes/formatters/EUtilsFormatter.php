<?php

use Drupal\Core\Url;
use Drupal\Core\Link;

/**
 * EUtilsFormatter Class.
 *
 * @defgroup formatters
 */
abstract class EUtilsFormatter {

  /**
   * Format a parser's output.
   *
   * @param array $data
   *   The data array returned by a parser.
   *
   * @return array
   *   This function does not return anything. It directly manipulates the
   *    elements array.
   */
  abstract public function format(array $data);

  /**
   * Fetch the DB object for an NCBI DB. Case insensitive.
   *
   * @param string $db_name
   *   The DB name as passed by the parser.
   * @param Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoDbxrefBuddy $dbxref_instance
   *   A chado buddy dbxref instance
   *
   * @return array
   *   Returns an array of Drupal\tripal_chado\ChadoBuddy\ChadoBuddyRecord,
   *   which will be empty if $db_name was not found.
   */
  public function getNCBIDB(string $db_name, $dbxref_instance) {
    $name = 'NCBI ' . $db_name;

    // Special case handling
    if (strtolower($db_name) == 'organism') {
      $name = 'NCBITaxon';
    }

    // First try as case sensitive, in case there might be two matches
    $db = $dbxref_instance->getDb(['db.name' => $name], []);
    if (!$db) {
      // No match, so now try case insensitive
      $db = $dbxref_instance->getDb(['db.name' => $name], ['case_insensitive' => 'db.name']);
    }

    // Finally, try without 'NCBI ' prefix
    if (!$db) {
      // No match, so now try case insensitive
      $db = $dbxref_instance->getDb(['db.name' => $db_name], ['case_insensitive' => 'db.name']);
    }

    return $db;
  }

  /**
   * Generates a URL link given a db string and accession.
   *
   * @param string $accession
   *   Accession string.
   * @param string $db_name
   *   Database lookup string.
   * @param bool $show_db
   *   Set to TRUE to include $db_name in the generated link
   *
   * @return mixed
   *   returns either the accession string, or a render array with
   *   a link to the xref.
   */
  public function getDbLink(string $accession, string $db_name, bool $show_db = FALSE) {
    $buddy_service = \Drupal::service('tripal_chado.chado_buddy');
    /** @var Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoDbxrefBuddy **/
    $dbxref_instance = $buddy_service->createInstance('chado_dbxref_buddy', []);

    // Note that getNCBIDB() is not case sensitive
    $db_records = $this->getNCBIDB($db_name, $dbxref_instance);

    $display_value = ($show_db?($db_name . ':'):'') . $accession;
    if (count($db_records)) {
      $db_records[0]->setValue('dbxref.accession', $accession);
      $url_string = $dbxref_instance->getDbxrefUrl($db_records[0]);
      $link = Link::fromTextAndUrl($display_value, Url::fromUri($url_string,
        ['attributes' => ['target' => '_blank']]));
      return $link;
    }
    else {
      return $display_value;
    }
  }

}
