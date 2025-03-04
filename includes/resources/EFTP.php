<?php

/**
 * Right now responsible for getting a single value from the Assembly.
 *
 * @ingroup resources
 */
class EFTP {

  /**
   * The FTP URL.
   *
   * @var string
   */
  protected $url;

  /**
   * Contains the content of the downloaded file.
   *
   * @var string
   */
  protected $file;

  /**
   * Get the contents of a file at a given URL.
   *
   * @param string $url
   *   The FTP URL.
   *
   * @throws \Exception
   */
  public function setURL($url) {

    $this->url = $url;
    // Because of occasional intermittent problems with FTP downloads, wrap
    // this download in a retry loop with a 3 second timeout and 20 retries.
    $maxtries = 20;
    $original_timeout = ini_get('default_socket_timeout');
    ini_set('default_socket_timeout', 3);
    $file = '';
    while (($maxtries) and (!$file)) {
      $file = file_get_contents($url);
      $maxtries--;
      if ((!$file) and ($maxtries)) {
        tripal_report_error('tripal_eutils', TRIPAL_WARNING, 'FTP download error, retrying !maxtries more times',
          ['!maxtries' => $maxtries], ['print' => TRUE, 'job' => $this->job]);
      }
    }
    // Set FTP timeout back to default
    if ($original_timeout) {
      ini_set('default_socket_timeout', $original_timeout);
    }
    if (!$file) {
      throw new Exception('Unable to connect to FTP resource: ' . $url);
    }
    $this->file = $file;
  }

  /**
   * Find all records of line starting with a specific item.
   *
   * @param string $field
   *   The field is the substring to look for at the start of a line.
   *
   * @return array
   */
  public function getField(string $field) {
    $results = [];

    $file = $this->file;

    $lines = explode("\n", $file);

    foreach ($lines as $line) {

      if (strpos($line, $field) === 0) {

        $string = str_replace($field, '', $line);

        $string = trim($string);

        $results[] = $string;
      }
      // If (!substr($line, 0, 1) === "#") {
      //        //we're done with the header, stop parsing.
      //        continue;
      //      }.
    }

    return $results;
  }

}
