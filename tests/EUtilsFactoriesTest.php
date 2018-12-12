<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

/**
 * Class EUtilsFactoriesTest.
 *
 * @package Tests
 */
class EUtilsFactoriesTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * @var array
   */
  protected $valid_dbs = [
    'biosample',
    'bioproject',
    'assembly',
  ];

  /**
   * @var array
   */
  protected $invalid_dbs = [
    'not_in_a_million_years',
  ];

  /**
   * @group factory
   * @throws \Exception
   */
  public function testThatEUtilsRepositoryFactoryReturnsARepository() {
    foreach ($this->valid_dbs as $db) {
      $repo = (new \EUtilsRepositoryFactory())->get($db);
      $this->assertInstanceOf(\EUtilsRepository::class, $repo);
    }
  }

  /**
   * @group factory
   * @throws \Exception
   */
  public function testThatEUtilsRepositoryFactoryThrowsAnExceptionWithInvalidDBs() {
    foreach ($this->invalid_dbs as $db) {
      $this->expectException(\Exception::class);
      (new \EUtilsRepositoryFactory())->get($db);
    }
  }

  /**
   * @group factory
   * @throws \Exception
   */
  public function testThatEUtilsFormatterFactoryReturnsAFormatter() {
    // Foreach ($this->valid_dbs as $db) {.
    // TODO: rewrite the test for all dbs once the formatters are ready.
    $db = 'biosample';
    $formatter = (new \EUtilsFormatterFactory())->get($db);
    $this->assertInstanceOf(\EUtilsFormatter::class, $formatter);
    // }.
  }

  /**
   * @group factory
   * @throws \Exception
   */
  public function testThatEUtilsFormatterFactoryThrowsAnExceptionWithInvalidDBs() {
    foreach ($this->invalid_dbs as $db) {
      $this->expectException(\Exception::class);
      (new \EUtilsFormatterFactory())->get($db);
    }
  }

  /**
   * @group factory
   * @throws \Exception
   */
  public function testThatEUtilsXMLParserFactoryReturnsAnXMLParser() {
    foreach ($this->valid_dbs as $db) {
      $formatter = (new \EUtilsXMLParserFactory())->get($db);
      $this->assertInstanceOf(\EUtilsParserInterface::class, $formatter);
    }
  }

  /**
   * @group factory
   * @throws \Exception
   */
  public function testThatEUtilsXMLParserFactoryThrowsAnExceptionWithInvalidDBs() {
    foreach ($this->invalid_dbs as $db) {
      $this->expectException(\Exception::class);
      (new \EUtilsXMLParserFactory())->get($db);
    }
  }

}
