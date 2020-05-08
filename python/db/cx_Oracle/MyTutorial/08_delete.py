"""
Demonstrate deletion from table updating using input parameter

Before running this demo, run demo 04_insert.py one or more times, to create some records in the table
This demo will delete all records created by 04_insert.py, including the records that has been updated by 06_update.py
"""
import cx_Oracle
import dbconfig

connection = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = connection.cursor()

sql = r"""
begin
    delete from REQUESTS where DETAILS in (:i_newDetails, :i_oldDetails);
    commit;
end;
"""
params = {
    ':i_oldDetails' : 'Sample request text',
    ':i_newDetails' : 'Updated request text'
    }
cursor.execute(sql, params)

connection.close()
print("Now see REQUESTS table content to check that records are deleted")
