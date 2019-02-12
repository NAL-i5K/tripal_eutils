<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class EUtilsTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  public function testSettingDB() {
    $connection = new \EUtils();
    $this->expectException('Exception');
    $connection->get('waffles', ['000000']);
  }


  /**
   * Accessions with letters arent UIDs and should be converted.
   *
   * @group eutils
   * @group convert
   * @dataProvider accessionDataProvider
   * @throws \Exception
   */
  public function testConvertingAccession($db, $input, $expect) {
    $connection_master = new \EUtils();
    $connect = reflect($connection_master);
    $out = $connect->convertAccessionsToUID($db, $input);
    $this->assertEquals($expect, $out);
    sleep(.4);
  }

  public function accessionDataProvider() {

    return [
      ['assembly', 'GCF_000184155.1', '557018'],
      ['assembly', '557018', '557018'],
    ];
  }
}
