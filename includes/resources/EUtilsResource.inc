<?php

/**
 * Class EUtilsResource
 *
 * Provides a set of methods to interact with a response
 * from the EUtils API.
 */
class EUtilsResource{

  /**
   * HTTP Response.
   *
   * @var object
   */
  protected $response;

  /**
   * EUtilsResource constructor.
   *
   * @param object $response The object returned by drupal_http_request()
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
   */
  public function status() {
    return (int) $this->response->code;
  }

  /**
   * Check if the request is successful.
   *
   * @return bool
   */
  public function isSuccessful() {
    $status = $this->status();
    return $status >= 200 && $status <= 299;
  }

  /**
   * Get raw body response.
   *
   * @return string
   */
  public function originalBody() {
    return $this->response->data;
  }

  /**
   * Parse response into XML.
   *
   * @return \SimpleXMLElement
   */
  public function xml() {
    return simplexml_load_string($this->originalBody());
  }

  /**
   * Parse response into DOMDocument.
   *
   * @return \DOMDocument
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
   */
  public function originalResponseObject() {
    return $this->response;
  }
}
