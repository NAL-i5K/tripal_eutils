<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use EUtilsResource;

class EUtilsResponseTest extends TripalTestCase {

  protected $response_mock;

  public function setUp() {
    parent::setUp();

    $this->response_mock = (object) [
      'headers' => [],
      'code' => 200,
      'data' => '',
    ];
  }

  /** @test */
  public function testThatErrorsAreRepresentedInTheResponse() {
    $path = DRUPAL_ROOT . '/' . drupal_get_path('module', 'tripal_eutils').'/examples/error.xml';
    $this->response_mock->data = file_get_contents($path);

    $response = new EUtilsResource($this->response_mock);

    $this->assertTrue($response->hasError());
    $this->assertFalse($response->isSuccessful());
    $this->assertNotNull($response->errorMessage());
  }
}
