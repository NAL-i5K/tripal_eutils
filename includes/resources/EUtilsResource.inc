<?php

/**
 * Interacts with a response from the EUtils API.
 *
 * @group resources
 */
class EUtilsResource {

  /**
   * HTTP Response.
   *
   * @var object
   */
  protected $response;

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
    return $status >= 200 && $status <= 299;
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
    return simplexml_load_string($this->originalBody());
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
