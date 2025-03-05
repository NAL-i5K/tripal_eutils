<?php

/**
 * Right now responsible for getting a single value from the Assembly.
 *
 * @ingroup resources
 */
class EFTP {

  /**
   * Contains the contents of the downloaded file.
   *
   * @var string $file
   */
  protected string $file;

  /**
   * Get the contents of a file at a given URL.
   *
   * @param string $url
   *   The FTP URL
   * @param bool $local
   *   TRUE if the $url represents a file on the local filesystem.
   *   This is only used for testing.
   *
   * @return void
   *   Saves the results in $this->file
   *
   * @throws \Exception
   */
  public function getURL(string $url, bool $local = FALSE): void {

    $logger = \Drupal::service('tripal.logger');
    $http_client = \Drupal::httpClient();

    //@todo add this to the settings form
    $retry_count = \Drupal::config('tripal_eutils.settings')->get('tripal_eutils.retry_count') ?? 10;
    $retry_wait = \Drupal::config('tripal_eutils.settings')->get('tripal_eutils.retry_wait') ?? 2;

    // Because of occasional intermittent problems with remote downloads, wrap
    // the download in a retry loop with a configurable number of retries.
    // @todo In the Tripal 3 version, we temporarily reduced the timeout to 3 seconds
    $file = '';
    while (($retry_count) and (!$file)) {
      $retry_count--;

      try {
        // $local is only used for PHPunit testing
        if ($local) {
          $file = file_get_contents($url) ?: '';
        }
        else {
          $response = $http_client->get($url);
          $file = (string) $response->getBody()->getContents();
        }
      }
      catch (\Exception $e) {
        // Do nothing here except wait, we will just retry
        if ($retry_count) {
          sleep($retry_wait);
        }
      }

      if ((!$file) and ($retry_count)) {
        $logger->warning('Remote site download problem, retrying @retry_count more times',
          ['@retry_count' => $retry_count]);
      }
    }
    //@todo this should not be an exception
    if (!$file) {
      throw new Exception(t('Unable to connect to external resource: @url', ['@url' => $url]));
    }
    $this->file = $file;
  }

  /**
   * Find all records of lines starting with a specific item.
   *
   * @param string $field
   *   This is the substring to look for at the start of a line.
   *
   * @return array
   *  The matching lines, with $field removed
   */
  public function getField(string $field): array {
    $results = [];
    $lines = explode("\n", $this->file);
    foreach ($lines as $line) {
      if (strpos($line, $field) === 0) {
        $string = str_replace($field, '', $line);
        $string = trim($string);
        $results[] = $string;
      }
    }
    return $results;
  }

}
