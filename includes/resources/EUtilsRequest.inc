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

  /**
   * Check whether the API key is still valid
   * 
   * @param string $api_key
   * @param string $db_url
   * 
   * @return bool
   */
  protected function api_key_validate($api_key) {
    // Get the value of tripal_eutils_recent_validation [last_validation, value]
    $recent_validation = variable_get('tripal_eutils_recent_validation',NULL);
    if ($recent_validation)
    {
      $now = intval(date("u"));
      // At some point, the API key was positively validated
      if ($recent_validation[1])
      {
        // Test if expired
        if ($now - $recent_validation[1] > 1800)
        {
          return false;
        }
        else
        {
          return true;
        }
      }
      // The API key was most recently found to be invalid
      else {
        return true;
      } 
    }
    else  // No known API validations or API validation has expired
    {
      $now = intval(date("u"));
      $test_url = "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/einfo.fcgi";
      $test_url = $test_url . '?api_key=' . $api_key;

      // Make a request to NCBI, collect and parse the response
      $response = drupal_http_request($test_url);

      if (array_key_exists('error',$response)) {
        // Some error exists with the API key. Print this
        // Set the recent
        variable_set('tripal_eutils_recent_validation',[$now,FALSE]);
        // and return false
        return false;
      }
      else {
        // Set the recent
        variable_set('tripal_eutils_recent_validation',[$now,TRUE]);
        return true;
      }

    }
  }

}
