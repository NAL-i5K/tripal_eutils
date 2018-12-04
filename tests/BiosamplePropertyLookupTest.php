<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class BiosamplePropertyLookupTest extends TripalTestCase{

  // Uncomment to auto start and rollback db transactions per test method.
  // use DBTransaction;

  /**
   * @group propertylookup
   */
  public function testBiosamplePropertyLookup() {
    $lookup = new \BiosamplePropertyLookup();
    $terms = $lookup->lookupAll();
    $this->assertNotEmpty($terms);
    var_dump($terms);

    // Version 1.0 as of November 29 2018: 456 terms.
    $this->assertEquals(456, count($terms));
  }

  /**
   * Random example of a term that should have been installed ni hte XML
   * reading.
   *
   * @group terms
   * @group propertylookup
   */
  public function testTermsInserted() {
    $term = chado_get_cvterm(['id' => 'ncbi_properties:fao_class']);
    $this->assertObjectHasAttribute('cvterm_id', $term);
    $this->assertObjectHasAttribute('name', $term);
    $this->assertEquals('FAO classification', $term->name);
  }
}
