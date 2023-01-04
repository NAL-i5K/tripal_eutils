<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class EutilsAssemblyXMLParserTest extends TripalTestCase {

  use DBTransaction;

  /**
   * AssemblyProvider creates array of XML and keys values to test.
   */
  protected $assembly_xmls = NULL;


  /**
   * @group assembly
   * @dataProvider AssemblyProvider
   *
   * @param $path - the path to the xml file
   * @param $base_keys = key => value pairs.  used to check that each key
   *   contains what is expected.
   *
   * @todo add some key value pairs for base_keys
   */

  public function testAssemblyParser($path, $base_keys) {

    $parser = $this->getMockBuilder('\EUtilsAssemblyParser')
      ->setMethods(['getFTPData'])
      ->getMock();
    //We mock the FTP call to speed up the test.
    $ftp_response = ['# Assembly method:' => 'a method, v1.0'];
    $parser->expects($this->once())
      ->method('getFTPData')
      ->will($this->returnValue($ftp_response));

    $assembly = $parser->parse(simplexml_load_file($path));

    $this->assertArrayHasKey('name', $assembly);
    $this->assertArrayHasKey('accessions', $assembly);
    $this->assertArrayHasKey('attributes', $assembly);
    $this->assertArrayHasKey('description', $assembly);
    $this->assertTrue(is_array($assembly['attributes']));
    $this->assertArrayHasKey('ignored', $assembly);
    $this->assertArrayHasKey('category', $assembly);

    $attributes = $assembly['attributes'];

    $this->assertArrayHasKey('stats', $attributes);
    $this->assertArrayHasKey('files', $attributes);
    $this->assertArrayHasKey('ftp_attributes', $attributes);

    $this->assertArrayHasKey('# Assembly method:', $attributes['ftp_attributes']);
    $this->assertNotNull($attributes['ftp_attributes']['# Assembly method:']);

    $this->assertNotNull($assembly['name']);
    $this->assertNotNull($assembly['accessions']);
    $this->assertNotNull($assembly['attributes']);
    $this->assertNotNull($assembly['description']);


    $accessions = $assembly['accessions'];

    $acc_master = $base_keys['accessions'];

    if (!empty($acc_master)) {
      //we only bother specifying keys for some of the files.
      $this->assertEquals($acc_master, $accessions);
    }

  }

  /**
   *
   */
  public function AssemblyProvider() {


    if ($this->assembly_xmls) {
      return $this->assembly_xmls;
    }


    $path = DRUPAL_ROOT . '/' . drupal_get_path('module', 'tripal_eutils');

    $files = [
      [
        $path . "/examples/assembly/1949871_assembly.xml",
        [

        ],
      ],
      [
        $path . "/examples/assembly/2004951_assembly.xml",
        [

        ],
      ],
      [
        $path . "/examples/assembly/317138_assembly.xml",
        [

        ],
      ],
      [
        $path . "/examples/assembly/524058_assembly.xml",
        [
          'name' => '',
          'sourcename' => 'SAMN00744358',
          'accessions' => [
            'assembly' => [
              'Refseq Assembly' => '817508',
              'Genbank Assembly' => '524058',
              'WGS' => 'AGSK01',
              'NCBI Assembly' => 'GCF_000298355.1'
            ],
            'taxon_accession' => '72004',
            'bioprojects' => ['74739', '221623'],
            'biosamples' => ['744358'],
          ],
          'submission_date' => '2013/01/09 00:00',
          'description' => '',
        ],
      ],
      [
        $path . "/examples/assembly/557018_assembly.xml",
        [],
      ],
      [
        $path . "/examples/assembly/559011_assembly.xml",
        [],
      ],
      [
        $path . "/examples/assembly/751381_assembly.xml",
        [

        ],
      ],
      [
        $path . "/examples/assembly/91111_assembly.xml",
        [

        ],
      ],
    ];

    $this->assembly_xmls = $files;
    return $files;
  }

  /**
   * @param $links
   * @param $expect
   * @dataProvider FTPURLProvider
   * @group ftp
   * @throws \Exception
   */
  public function testFTPGuesser($links, $expect){
    $parent  = new \EUtilsAssemblyParser();
    $p = reflect($parent);

  $link =     $p->guessFTPUrl($links);
  $this->assertEquals($expect, $link);

  }

  public function FTPURLProvider() {

    return [
      [[], NULL],
      [['dont_use_this' => 'dog'], NULL],
      [['Assembly_stats' => 'ftp://ftp.ncbi.nlm.nih.gov/genomes/all/A/B/accession/somefile.txt'], 'ftp://ftp.ncbi.nlm.nih.gov/genomes/all/A/B/accession/somefile.txt'],
      [['Assembly_stats' => 'ftp://ftp.ncbi.nlm.nih.gov/genomes/all/A/B/accession/somefile.txt',
        'RefSeq' => "ftp://dontuseme.gov/folder/"],
        'ftp://ftp.ncbi.nlm.nih.gov/genomes/all/A/B/accession/somefile.txt'],
      [['RefSeq' => 'ftp://ftp.ncbi.nlm.nih.gov/genomes/all/A/B/guess/'], 'ftp://ftp.ncbi.nlm.nih.gov/genomes/all/A/B/guess/guess_assembly_stats.txt'],


    ];

  }

}
