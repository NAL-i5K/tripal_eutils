<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class EUtilsTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * @group eutils
   * @throws \Exception
   */
  public function testSettingDB() {
    $connection = new \EUtils();
    $this->expectException('Exception');
    $connection->get('waffles', '000000');
  }


  /**
   * Accessions with letters arent UIDs and should be converted.
   *
   * @group eutils
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
      ['assembly', 'dog', FALSE],
      ['assembly', 'DFDSJKFLADSFKDASFFOIHWINBIANBSDFKJASLFKJ', FALSE],
      ['bioproject', 'PRJNA66853', '66853'],
      ['biosample', 'SAMN02981385', '2981385']


    ];
  }
//
//  /**
//   * @group orange
//   */
//  public function testChadoInsertRecordCanJoin(){
//
//    $pub = factory('chado.pub')->create();
//    $analysis = factory('chado.analysis')->create();
//
//    chado_insert_record('analysis_pub', ['pub_id' => $pub->pub_id, 'analysis_id' => $analysis->analysis_id]);
//    $connection = chado_db_select('pub', 'p');
//    $connection->join('{analysis_pub}', 'ap', 'ap.pub_id = p.pub_id');
//    $connection->fields('p');
//    $connection->condition('ap.pub_id', $analysis->analysis);
//    $result = $connection->execute()->fetchObject();
//
//    $this->assertNotFalse($result);
//
//    var_dump($result);
//
//    $this->assertEquals($result->pub_id, $pub->pub_id);
//
//  }
}
