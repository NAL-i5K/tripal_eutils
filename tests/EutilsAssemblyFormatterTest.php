<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class EutilsAssemblyFormatterTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  // use DBTransaction;

  public function testWholeFormat(){

    $parsed = $this->parseXML();
    $elements = [];
    $formatter = new \EutilsAssemblyFormatter();
    $formatter->format($parsed, $elements);
  }

  private function parseXML() {

    $file = __DIR__ . '/../examples/assembly/559011_assembly.xml';
    $parser = $this->getMockBuilder('\EUtilsAssemblyParser')
      ->setMethods(['getFTPData'])
      ->getMock();

    // We mock the FTP call.
    $ftp_response = ['# Assembly method:' => 'a method, v1.0'];

    $parser->expects($this->once())
      ->method('getFTPData')
      ->will($this->returnValue($ftp_response));

    $assembly = $parser->parse(simplexml_load_file($file));

    return $assembly;
  }
}
