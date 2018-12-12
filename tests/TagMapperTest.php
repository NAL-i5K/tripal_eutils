<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

/**
 *
 */
class TagMapperTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * @group attributes
   */
  public function testClassInits() {

    $mapper = new \TagMapper('biosample');

    $term = $mapper->lookup('sex');

    $this->assertNotNull($term);
  }

  /**
   *
   */
  public function testLabelLooker() {

    $mapper = new \TagMapper('biosample');

    $hname = 'cow';
    $aname = 'dog';
    $dname = 'display';
    $value = 'some_value';
    $term = [
      'value' => $value,
      'harmonized_name' => $hname,
      'attribute_name' => $aname,
      'display_name' => $dname,
    ];
    $picked = $mapper->getDisplayLabel($term);

    $this->assertEquals($dname, $picked);

    unset($term['display_name']);
    $picked = $mapper->getDisplayLabel($term);

    $this->assertEquals($aname, $picked);

    unset($term['attribute_name']);

    $picked = $mapper->getDisplayLabel($term);

    $this->assertNotFalse($picked);
    $this->assertNotEquals($value, $picked);
  }

}
