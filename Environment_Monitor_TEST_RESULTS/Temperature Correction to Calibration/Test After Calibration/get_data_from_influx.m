clc;clear;close all;

options_read = weboptions('RequestMethod','GET');
my_query = [ 'SELECT * FROM "environment" WHERE time > ''2023-03-27T11:06:40.000000000Z'''];
returned_data = webread('http://192.168.0.19:8086/query?db=arduino&u=grafana&p=physics1234','q',my_query,options_read);
time = datetime(cellfun(@(x)x(1),returned_data.results.series.values),'InputFormat','yyyy-MM-dd''T''HH:mm:ss.SSSSSSSSS''Z''');
BX = cell2mat(cellfun(@(x)x(2),returned_data.results.series.values));
BY = cell2mat(cellfun(@(x)x(3),returned_data.results.series.values));
BZ = cell2mat(cellfun(@(x)x(4),returned_data.results.series.values));
Temperature = cell2mat(cellfun(@(x)x(5),returned_data.results.series.values));

% Put the data into a Matlab table object
data_TT = timetable(time,BX,BY,BZ,Temperature);
% Calculate number of hours since midnight on 1st Jan 2023
data_TT.time_hours = hours( data_TT.time - datetime("2023-01-01 00:00:00") );
clear my_query returned_data time data BX BY BZ


figure;
plot( data_TT.time , data_TT.BX , '.-' )

figure;hold all;
plot( data_TT.time , data_TT.BY , '.-' )
plot( data_TT.time , data_TT.BZ , '.-' )

figure;
plot( data_TT.time , data_TT.Temperature , '.-' )



