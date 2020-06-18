"""
Demonstrate working with json data type

TODO: upgrade to ORacle 12.2 or higher and complete this tutorial
"""
import cx_Oracle
import dbconfig
import json

connection = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = connection.cursor()


sql = r"""
begin
    insert into EXAMPLE_JSON (DT, JSON_DATA) values (sysdate, :json_data);
    commit;
end;
"""

logRecord = {"server":"c7cls01", "app":"BaseApp", "instance":1, "severity":"INFO", "descr":"Some log message", "metainfo":{"field1":"value1", "field2":"value2"}}
params = {':json_data' : json.dumps(logRecord)}
cursor.execute(sql, params)


connection.close()
print("Now see EXAMPLE_JSON table content to check that new record is inserted")
