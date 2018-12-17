Installation and Setup
=======================

.. warning::

	This module is still under active development.

Requirements
------------

Tripal EUtilities requires:

- Tripal 3
- PHP >= 7.0
- Drupal 7
- [Tripal Manage Analyses](https://github.com/statonlab/tripal_manage_analyses.git)

Installation
------------

``tripal_eutils`` and its dependency ``tripal_manage_analyses`` are not available for deployment via Drush and must be installed via git.

.. code-block:: shell

  cd [location of your custom or contrib modules]
  git clone https://github.com/statonlab/tripal_manage_analyses.git
  git clone https://github.com/NAL-i5K/tripal_eutils.git
  drush pm-enable tripal_manage_analyses tripal_eutils -y

Setup
-----
This module currently functions "as is" without setup.  The Manage Analyses module provides several new fields (analysis and organism linker fields) so you should **Check For New Fields** on the content types your site utilizes that have _organism or _analysis Chado linker tables.


Permissions
~~~~~~~~~~~~

This module only defines one permission: ``access tripal_eutils admin``.  This permission will allow users to use the admin form to directly insert Chado records into the database given NCBI accessions.  Because this form adds data to your db, we suggest reserving it for administrators.
