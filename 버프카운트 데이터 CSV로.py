import pandas as pd

# 출력 옵션 변경
pd.set_option('display.max_rows', None)

# CSV 파일 읽어오기
data = pd.read_csv('C:/Users/KOSA/Naver MYBOX/Cording Study/Kosa/Code/C++/WAiSER Use Project/최종 프로젝트/DT_FINAL_Project/CSV/buf_count_list2.csv')

for i in range(1, 6):
    # current_time이 같을 시, buffer_count와 stock_count 는 가장 큰 값만을 사용
    data1 = data.groupby(['project_id', 'object_id', 'object_type', 'current_time']).max().reset_index()

    # object_id 별로 buffer_count와 stock_count 의 평균 구하기
    result = data1.groupby(['project_id', 'object_id', 'object_type']).mean().reset_index()[['project_id', 'object_id', 'object_type', 'buffer_count', 'stock_count']]
    result.to_csv(f'mean_by_object_id_{i}.csv', index=True)

    df_mean = data1.groupby('object_type')[['buffer_count', 'stock_count']].mean()
    df_mean.to_csv(f'mean_by_object_type_{i}.csv', index=True)

# 모든 결과를 하나의 csv 파일에 저장
df_object_id = pd.DataFrame()
df_object_type = pd.DataFrame()
for i in range(1, 5):
    df_object_id_i = pd.read_csv(f'mean_by_object_id_{i}.csv')
    df_object_id = pd.concat([df_object_id, df_object_id_i], axis=0)
    
    df_object_type_i = pd.read_csv(f'mean_by_object_type_{i}.csv')
    df_object_type = pd.concat([df_object_type, df_object_type_i], axis=0)

df_object_id.to_csv('mean_by_object_id.csv', index=False)
df_object_type.to_csv('mean_by_object_type.csv', index=False)
