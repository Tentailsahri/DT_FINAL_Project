@echo off
psql -d "postgresql://192.168.0.18/final_DB?user=postgres&password=1234" -c "\copy project_list1 to 'C:/project_list1.csv' with csv header;"
psql -d "postgresql://192.168.0.18/final_DB?user=postgres&password=1234" -c "\copy project_list2 to 'C:/project_list2.csv' with csv header;"