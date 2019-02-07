<?php

/**
 * Interacts with a response from the EUtils API.
 *
 * @ingroup resources
 */
class EUtilsResource {

  /**
   * HTTP Response.
   *
   * @var object
   */
  protected $response;

  /**
   * Holds the error message if any exists or null.
   *
   * @var string|null
   */
  protected $error = NULL;

  /**
   * The xml cache.
   *
   * @var \SimpleXMLElement|null
   */
  protected $xml = NULL;

  /**
   * EUtilsResource constructor.
   *
   * @param object $response
   *   The object returned by drupal_http_request()
   *
   * @see drupal_http_request()
   */
  public function __construct($response) {
    $this->response = $response;
  }

  /**
   * Get an array of response headers.
   *
   * @return array
   */
  public function headers() {
    return $this->response->headers;
  }

  /**
   * Get the response status code.
   *
   * @return int
   *   Status code.
   */
  public function status() {
    return (int) $this->response->code;
  }

  /**
   * Check if the request is successful.
   *
   * @return bool
   *   TRUE for success.
   */
  public function isSuccessful() {
    $status = $this->status();

    if ($status == 200) {
      if ($this->hasError()) {
        return FALSE;
      }

      if ($this->xml()) {
        return FALSE;
      }
    }

    return $status >= 200 && $status <= 299;
  }

  /**
   * Find and set errors.
   *
   * @return bool
   *   Whether the response has an error element.
   */
  public function hasError() {
    $xml = $this->xml();

    if ($xml && isset($xml->DocumentSummarySet->DocumentSummary->error)) {
      $this->error = (string) $xml->DocumentSummarySet->DocumentSummary->error;
      return TRUE;
    }

    if ($xml && isset($xml[0])) {
      if (strpos((string) $xml->RecordSet, 'malformed') == 0) {
        $this->error = 'Unable to find record.';
        return TRUE;
      }
    }

    return FALSE;
  }

  /**
   * Get the error message.
   *
   * @return string|null
   *   The message or null if none exist.
   */
  public function errorMessage() {
    return $this->error;
  }

  /**
   * Get raw body response.
   *
   * @return string
   *   The raw body response string.
   */
  public function originalBody() {
    return $this->response->data;
  }

  /**
   * Parse response into XML.
   *
   * @return \SimpleXMLElement
   *   The response in XML.
   */
  public function xml() {
    // Check cache.
    if (!is_null($this->xml)) {
      return $this->xml;
    }

    // Cache xml
    return $this->xml = simplexml_load_string($this->originalBody());
  }

  /**
   * Parse response into DOMDocument.
   *
   * @return \DOMDocument
   *   The response as  DOMDocument.
   */
  public function dom() {
    $dom = new DOMDocument();
    $dom->loadXML($this->originalBody());
    return $dom;
  }

  /**
   * Get the response object.
   *
   * @return object
   *   The original response object.
   */
  public function originalResponseObject() {
    return $this->response;
  }

}
