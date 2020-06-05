# Python - Oracle integration tutorial

## Installation

* Get access to Oracle database and create a scheme
* Install Oracle client and configure tnsname to access the database (for example, 'testdb')
* Create scheme in DB where tests will be done (for example, user 'test' with password 'test')
* Create DB objects in the newly created test scheme (package source are provided at Prerequisites\cx_oracle_tutorial_db_objects)
* Install cx_Oracle
	py -3 -m pip install cx_oracle

## Configuration
* The only required configuration step is to provide connection parameters in file dbconfig.py

## Tutorial
* Run the example python scripts, read the info provided in file comments and follow the instructions

## References:
[1]: https://cx-oracle.readthedocs.io/en/latest/index.html
