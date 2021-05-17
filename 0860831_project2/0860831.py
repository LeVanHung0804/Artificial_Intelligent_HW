#hungle: 0860831
from math import log, sqrt, pi
import pandas as pd
import numpy as np

def import_data(file_name):

    print("import data name: ", file_name)
    data = pd.read_excel(file_name)

    #drop column No
    data.drop('No', axis=1, inplace=True)
    
    # replace 2 in 'Gender' to 0
    for row_th, data_th in data.iterrows():
        if data_th['Gender'] == 2:
            data.loc[row_th,'Gender'] = 0
            
    print('data.shape :', data.shape)
    return data

def reshape_data(data):
    num_class_all = [0 , 0]
    new_data = data
    new_data_0 = []
    new_data_1 = []
    fold_size = len(data)//10
    num_0_per_fold = 23
    num_1_per_fold = 7
    for row_th, data_th in data.iterrows():
        num_class_all[data_th['Target']] += 1

    for row_th, data_th in data.iterrows():
        if data_th['Target'] == 0:
            new_data_0.append(data_th)
        else:
            new_data_1.append(data_th)
    
    i = 0
    while (i < 10):
        new_data.loc[0+30*i:22+30*i,:] = new_data_0[0+23*i:23+23*i]
        new_data.loc[23+30*i:29+30*i,:] = new_data_1[0+7*i:7+7*i]       
        i +=1    
    return new_data

def get_mean_feature(data,feature):
    sum = 0
    count = 0
    for row_th, data_th in data.iterrows():
        value = data_th[feature]
        if np.isnan(value):
            continue
        sum += value
        count +=1
    return sum/count

class K_fold_NB(): #k = 10

    def __init__(self):
        self.mean, self.variance, self.category,self.num_class  = [{}, {}], [{}, {}], [{}, {}], [0, 0]

    def reset_model(self):
        self.mean, self.variance, self.category, self.num_class = [{}, {}], [{}, {}], [{}, {}], [0, 0]

    def save_model(self):
        self.saved_mean = self.mean
        self.saved_variance = self.variance
        self.saved_category = self.category
        self.saved_num_class = self.num_class
        self.saved_min_category = self.min_category

    def train_model(self, dataset, start_test, end_test):

        # init mean, variance, category of each feature
        for feature, feature_data in dataset.iteritems():
            if feature in NUMERICAL_LIST:
                self.mean[0][feature] = 0
                self.mean[1][feature] = 0
                self.variance[0][feature] = 0
                self.variance[1][feature] = 0
            else:
                self.category[0][feature] = {}
                self.category[1][feature] = {}

        # count number of class 0 and number of class 1
        for row_th, data_th in dataset.iterrows():

            if row_th >= start_test and row_th <= end_test:
                continue
            self.num_class[data_th['Target']] += 1

        #calculate sum of data to calculate mean in next step
        for row_th, data_th in dataset.iterrows():

            if row_th >= start_test and row_th <= end_test:
                continue

            label = data_th['Target'] # 0 or 1
            for feature, value in data_th.items():
                if feature in NUMERICAL_LIST:
                    if np.isnan(value):
                        value = get_mean_feature(training_set,feature)
                    self.mean[label][feature] += value
                else:
                    # if no catogory in the first time, init category
                    if value not in self.category[0][feature]: 
                        self.category[0][feature][value] = 0
                    if value not in self.category[1][feature]:
                        self.category[1][feature][value] = 0
                        
                    self.category[label][feature][value] += 1

        #avoid log(0) case
        self.min_category = 1000000000
        for row_th, data_th in dataset.iterrows():

            if row_th >= start_test and row_th <= end_test:
                continue

            for feature, value in data_th.items():
                if feature not in NUMERICAL_LIST:
                    if (self.category[0][feature][value] > 0.0001):
                        self.min_category = min(self.min_category, self.category[0][feature][value])
                    if (self.category[1][feature][value] > 0.0001):
                        self.min_category = min(self.min_category, self.category[1][feature][value])

        #calculate mean
        for feature, feature_data in dataset.iteritems(): # DUYET TUNG COT
            if feature in NUMERICAL_LIST:
                self.mean[0][feature] /= self.num_class[0]
                self.mean[1][feature] /= self.num_class[1]

        #calculate variance
        for row_th, data_th in dataset.iterrows():

            if row_th >= start_test and row_th <= end_test:
                continue

            label = data_th['Target']
            for feature, value in data_th.items():
                if feature in NUMERICAL_LIST:
                    if np.isnan(value):
                        value = get_mean_feature(training_set,feature)
                    self.variance[label][feature] += (value - self.mean[label][feature]) ** 2

        for feature, value in data_th.items(): #duyet tung cot
            if feature in NUMERICAL_LIST:
                self.variance[0][feature] /= (self.num_class[0] - 1)
                self.variance[1][feature] /= (self.num_class[1] - 1)

    def validate_model(self, dataset, start_test, end_test):
        num_true_predict = 0.0
        outputs = []

        for row_th, data_th in dataset.iterrows():
            if row_th < start_test or row_th > end_test:
                continue

            target = -1
            max_posterior = -1000000000
            prediction = -1

            for label in range(2):
                # print("label:", label)

                p_c = log(self.num_class[label] / (self.num_class[0] + self.num_class[1]))
                p_features_c = 0.0
                
                for feature, value in data_th.items():

                    if feature == 'Target':
                        target = value
                        continue

                    #calculate P of each feature when know class c 0 or 1
                    if feature in NUMERICAL_LIST:
                        if np.isnan(value):
                            value = get_mean_feature(training_set,feature)
                        p_features_c += (log(1.0 / sqrt(2 * pi * self.variance[label][feature])) 
                                        - ((value - self.mean[label][feature]) ** 2) / (2.0 * self.variance[label][feature]))
                    else:    
                        if value not in self.category[label][feature]:
                            self.category[label][feature][value] = 0

                        if self.category[label][feature][value] == 0:                            
                            self.category[label][feature][value] = self.min_category
                        
                        p_features_c += log(self.category[label][feature][value] / self.num_class[label])

                posterior = (p_features_c + p_c)

                if posterior > max_posterior:
                    max_posterior = posterior
                    prediction = label

            #check target
            if target != -1:
                num_true_predict += (prediction == target)

            outputs.append((row_th, prediction))

        if target != -1:
            print("Validating accuracy: %f%%" % (num_true_predict * 100 / (end_test - start_test + 1)))

        return num_true_predict * 100 / dataset.shape[0]

    def test_model(self, dataset):

        num_true_predict = 0.0
        outputs = []

        for row_th, data_th in dataset.iterrows():
            max_posterior = -1000000000
            prediction = -1

            for label in range(2):
                p_c = log(self.saved_num_class[label] / (self.saved_num_class[0] + self.saved_num_class[1]))
                p_features_c = 0.0
                
                for feature, value in data_th.items():
                    if feature in NUMERICAL_LIST:
                        if np.isnan(value):
                            value = get_mean_feature(training_set,feature)
                        p_features_c += (log(1.0 / sqrt(2 * pi * self.saved_variance[label][feature])) 
                                        - ((value - self.saved_mean[label][feature]) ** 2) / (2.0 * self.saved_variance[label][feature]))
                    else:    
                        if value not in self.saved_category[label][feature]:
                            self.saved_category[label][feature][value] = 0

                        if self.saved_category[label][feature][value] == 0:                            
                            self.saved_category[label][feature][value] = self.saved_min_category
                        
                        p_features_c += log(self.saved_category[label][feature][value] / self.saved_num_class[label])

                posterior = (p_features_c + p_c)

                if posterior > max_posterior:
                    max_posterior = posterior
                    prediction = label

            outputs.append((row_th, prediction))
        return outputs


TRAINING_FILE = "Project_NB_Tr.xlsx"

TESTING_FILE = "Project_NB Ts.xlsx"

NUMERICAL_LIST = [
    'ASA', 'Age',	'Height', 'BW',	'BMI', 'Serum ESR', 'Serum WBC', 
    'Segment',	'HGB', 'PLATELET', 'P.T', 'APTT', 'Serum CRP',	
    'CR(B)', 'AST',	'Total CCI' , 'Total Elixhauser Groups per record'
]

if __name__ == "__main__":

    training_set = import_data(TRAINING_FILE)
    fold_size = len(training_set) // 10
    # reshape data flow the original distribution of dataset
    training_set = reshape_data(training_set)

    best_val_acc = 0
    model = K_fold_NB()
    #check acc in each fold: 10 fold, 30 data each fold
    #save model with the best acc
    for start_test in range(0, len(training_set)-fold_size+1, fold_size):
        print('=======================================================')
        print('Fold_th %d' % (start_test // fold_size))
        model.reset_model()
        model.train_model(training_set, start_test, start_test + fold_size)
        val_acc = model.validate_model(training_set, start_test, start_test + fold_size)
        
        if best_val_acc < val_acc:    
            model.save_model()
            best_val_acc = val_acc            

        print("fold index: ", start_test, " - " , start_test+fold_size)

    print("=================training process done=================")
    testing_set = import_data(TESTING_FILE)
    output_test = model.test_model(testing_set)
    for i in range(len(output_test)):
        print('index: ',output_test[i][0]+1, '  predic class: ', output_test[i][1])

