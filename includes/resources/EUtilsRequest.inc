<?php

/**
 * Builds and executes the API request to NCBI.
 *
 * @ingroup resources
 */
class EUtilsRequest {

  /**
   * Request base url.
   *
   * @var string
   */
  protected $base_url;

  /**
   * Request headers.
   *
   * @var array
   */
  protected $headers = [];

  /**
   * Request parameters.
   *
   * @var array
   */
  protected $params = [];

  /**
   * @param string $url
   *
   * @return $this
   */
  public function setBaseURL($url) {
    $this->base_url = $url;
    return $this;
  }

  /**
   * @param $key
   * @param $value
   *
   * @return $this
   */
  public function addHeader($key, $value) {
    $this->headers[$key] = $value;
    return $this;
  }

  /**
   *
   * @param $headers
   *
   * @return $this
   */
  public function addHeaders($headers) {
    $this->headers = $headers + $this->headers;
    return $this;
  }

  /**
   * Add a single parameter.
   *
   * @param string $key
   * @param string $value
   *
   * @return $this
   */
  public function addParam($key, $value) {
    $this->params[$key] = $value;

    return $this;
  }

  /**
   * Add an array of parameters.
   *
   * @param $params
   *
   * @return $this
   */
  public function addParams($params) {
    $this->params = $params + $this->params;

    return $this;
  }

  /**
   * Send a GET request.
   *
   * @param string $url
   *
   * @return \EUtilsResource
   */
  public function get($url = '') {
    return $this->sendRequest('GET', $url);
  }

  /**
   * Send a POST request.
   *
   * @param string $url
   *
   * @return \EUtilsResource
   */
  public function post($url = '') {
    return $this->sendRequest('POST', $url);
  }

  /**
   * Send the request.
   *
   * @param $method
   * @param $path
   *
   * @return \EUtilsResource
   */
  protected function sendRequest($method, $path) {
    $url = $this->prepURL($path);

    $params = [
      'method' => $method,
      'headers' => $this->headers,
    ];

    if (!empty($this->params)) {
      if ($method !== 'POST') {
        $url .= '?' . http_build_query($this->params);
      }
      else {
        $params['data'] = http_build_query($this->params);
      }
    }

    $response = drupal_http_request($url, $params);

    return new EUtilsResource($response);
  }

  /**
   * Prepare the url.
   *
   * @param string $path
   *
   * @return string
   */
  protected function prepURL($path) {
    $path = trim($path, '/');

    if ($this->beginsWith($path, 'http')) {
      $url = $path;
    }
    else {
      $url = $this->base_url . "/$path";
    }

    return $url;
  }

  /**
   * Check whether a string begins with another string.
   *
   * @param string $str
   * @param string $with
   *
   * @return bool
   */
  protected function beginsWith($str, $with) {
    return substr($str, 0, strlen($with)) === $with;
  }

}
