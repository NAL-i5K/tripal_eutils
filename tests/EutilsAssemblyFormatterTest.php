<?php

namespace Tests;

use StatonLab\TripalTestSuite\TripalTestCase;

/**
 *
 */
class EutilsAssemblyFormatterTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  // use DBTransaction;.

  /**
   * Run through the formatter and check key values match the test assembly XML.
   *
   * @group wip
   */
  public function testWholeFormat() {

    $parsed = $this->parseXML();
    $formatter = new \EutilsAssemblyFormatter();
    $elements = $formatter->format($parsed);

    $this->assertNotEmpty($elements);

    $this->assertArrayHasKey('base_record', $elements);
    $this->assertArrayHasKey('properties', $elements);

  }

  /**
   * Provide a mocked parsed Assembly XML file.
   *
   * @return array
   *   parsed XML array.
   */
  private function parseXML() {

    // Load in a test assembly and mock its FTP method/data.
    $file = __DIR__ . '/../examples/assembly/559011_assembly.xml';
    $parser = $this->getMockBuilder('\EUtilsAssemblyParser')
      ->setMethods(['getFTPData'])
      ->getMock();
    $ftp_response = ['# Assembly method:' => 'a method, v1.0'];

    $parser->expects($this->once())
      ->method('getFTPData')
      ->will($this->returnValue($ftp_response));

    $assembly = $parser->parse(simplexml_load_file($file));

    return $assembly;
  }

}
