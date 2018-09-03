function varargout = textGUI(varargin)
% TEXTGUI MATLAB code for textGUI.fig
%      TEXTGUI, by itself, creates a new TEXTGUI or raises the existing
%      singleton*.
%
%      H = TEXTGUI returns the handle to a new TEXTGUI or the handle to
%      the existing singleton*.
%
%      TEXTGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in TEXTGUI.M with the given input arguments.
%
%      TEXTGUI('Property','Value',...) creates a new TEXTGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before textGUI_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to textGUI_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help textGUI

% Last Modified by GUIDE v2.5 29-Aug-2018 15:49:33

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @textGUI_OpeningFcn, ...
                   'gui_OutputFcn',  @textGUI_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before textGUI is made visible.
function textGUI_OpeningFcn(hObject, eventdata, handles, varargin)



function HR_text_Callback(hObject, eventdata, handles)

a = get(handles.AVI_text,'String');
b = get(handles.VAI_text,'String');

total = str2num(a) + str2num(b);  % 格式转换，转换为数值
c = num2str(total); % 转换为字符串
set(handles.HR_text,'String',c);
guidata(hObject, handles); % 更新结构体


% --- Executes during object creation, after setting all properties.
function HR_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to HR_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in popupmenu1.
function popupmenu1_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu1 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu1


% --- Executes during object creation, after setting all properties.
function popupmenu1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function AVI_text_Callback(hObject, eventdata, handles)
input = str2num(get(hObject,'String'));
if (isempty(input))
     set(hObject,'String','0');
end
guidata(hObject, handles); 



% --- Executes during object creation, after setting all properties.
function AVI_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to AVI_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function VAI_text_Callback(hObject, eventdata, handles)
% hObject    handle to VAI_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

input = str2num(get(hObject,'String')); % string属性是字符串，所以必须转换成数值
if (isempty(input))
     set(hObject,'String','0');
end
guidata(hObject, handles); 

% Hints: get(hObject,'String') returns contents of VAI_text as text
%        str2double(get(hObject,'String')) returns contents of VAI_text as a double


% --- Executes during object creation, after setting all properties.
function VAI_text_CreateFcn(hObject, eventdata, handles)
global G_AVI G_VAI;
G_AVI=150;
G_VAI=850;
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pb_stop.
function pb_stop_Callback(hObject, eventdata, handles)
global  flag_stop request_all;
flag_stop=1;
request_all = 0;



function pb_add_Callback(hObject, eventdata, handles)

a = get(handles.AVI_text,'String');
b = get(handles.VAI_text,'String');

total = str2num(a) + str2num(b);  % 格式转换，转换为数值
c = num2str(total); % 转换为字符串
set(handles.HR_text,'String',c);
guidata(hObject, handles); % 更新结构体


% --- Executes on button press in pb_text.
function pb_text_Callback(hObject, eventdata, handles)
c='1030';
set(handles.VAI_text, 'String',c);


% --- Executes on button press in Connect.
function pb_conn_Callback(hObject, eventdata, handles)   
global t;
t = tcpip('192.168.0.67',8888);
fopen(t);                                                               %it create the socket and get connnection


% --- Executes on button press in pb_allRequest.
function pb_allRequest_Callback(hObject, eventdata, handles)
global t request_AVI request_all G_AVI G_VAI;
request_AVI=0;
request_all=1;
Ventricle_record{1,1}='AVI';
Atrial_record{1,1}='VAI';
VAI_blank{1,1}='VAI';
AVI_blank{1,1}='AVI';

listcount=0;
x=0;
y=0;
yy=0;
xcount=0;
axes(handles.axes1);   %draw on the  axes1
cla reset;
hold on;
xlim([0 7000])
ylim([-1.1 1.1])
xlabel('Time (ms)');
ylabel('Ventricular Pace          Atrial Pace');

axes(handles.axes2);         %draw on the  axes2
cla reset;
hold on;
xlim([0 7000])
ylim([0 180])
xlabel('Time (ms)');
ylabel('Heart rate (/min)');

% for i=1:10;
%     Atrial_record{1,i}='';
%     Ventricle_record{1,i}='';
% end
set(handles.listbox1,'String',Atrial_record);
set(handles.listbox2,'String',Ventricle_record);


while( request_all==1 )
    req_data='9';            %all data requested 
    fwrite(t,req_data);
    listcount=listcount+1;
%     if listcount==10
%         listcount=1;
%         for i=1:10;
%             Atrial_record{1,i}='';
%             Ventricle_record{1,i}='';
%         end
%     end
    for i=1:3
        if (i==1)                       %VAI data
            data=fread(t,4)';
            num=str2num(char(data));
            if(num>9000)
                num=num-9000;
            end
            if(num<G_VAI)
                Atrial_record{1,listcount}='Atrium sense';
            else
                Atrial_record{1,listcount}='Atrium pace';
            end
            VAI_record{1,listcount}=num2str(num);
            Data=cell(listcount,5);
            Data(:,1)=Atrial_record;
            Data(:,2)=VAI_record;
%             set(handles.uitable1, 'ColumnName', {'Atrium','VAI'}, 'data',Data);  
%             set(handles.listbox1,'String',Atrial_record);
        else
            data=fread(t,3)';       %AVI or heart rate data
            num=str2num(char(data));
        end
        if(i==1)                                                %plot atrial pace
            x1=xcount+1: xcount+num+10;     %the 'num' is the time of VAI
            xcount=xcount+num+10;               %10 is the error value used to show the peak
            x=[x x1];
            yy1=x1;
            y1=linspace(0,0,num);
            y2=linspace(1,1,10);            %y-axis show the pace, y=1 means atrial pace 
            y=[y y1 y2];
            axes(handles.axes1);   
            plot(x,y,'b');
        elseif(i==2)                                        % plot vertri pace 
            if(num>900)
                num=num-900;
            end
            
            if(num<G_AVI )
                Ventricle_record{1,listcount}='Ventricle sense';
            else
                Ventricle_record{1,listcount}='Ventricle pace';
            end 
            
            AVI_record{1,listcount}=num2str(num);
            Data(:,3)=Ventricle_record;
            Data(:,4)=AVI_record;
            set(handles.uitable1, 'ColumnName', {'Atrium','VAI','Ventricle','AVI'}, 'data',Data); 
%             set(handles.listbox2,'String',Ventricle_record);
            
            x1=xcount+1: xcount+num+10;
            xcount=xcount+num+10;
            if (x==0)                               % refresh the plot 
                yy1=x1;
            else
                yy1=[yy1 x1];
            end
            x=[x x1];
            
            y1=linspace(0,0,num);
            y2=linspace(-1,-1,10);          %y=-1 means ventricular pace 
            y=[y y1 y2];
            axes(handles.axes1);   
            plot(x,y,'b');
        end

        if (i==3)                           %plot heart rate
            if (num<300)
                heart_fast=1;
            else
                heart_fast=0;
                heartRate=num/10;
            end
            show_rate=num;
            if(show_rate>300)
                show_rate=show_rate/10;
            end
            Heartrate_record{1,listcount}=num2str(show_rate);
            Data(:,5)=Heartrate_record;
            set(handles.uitable1, 'ColumnName', {'Atrium','VAI','Ventricle','AVI','Heart rate'}, 'data',Data); 
            
            yy1=linspace(heartRate,heartRate,numel(yy1));            
            yy=[yy yy1];
            axes(handles.axes2);   
            plot(x,yy,'b');
        end
        if (xcount>7000)            %reset figure   
            xcount=0;
            x=0;
            y=0;
            yy=0;
            axes(handles.axes1);   
            cla reset;
            hold on;
            xlim([0 7000]);
            ylim([-1.1 1.1]);
            xlabel('Time (ms)');
            ylabel('Ventricular Pace          Atrial Pace');
       
            axes(handles.axes2);   
            cla reset;
            hold on;
            xlim([0 7000]);
            ylim([0 180]);
            xlabel('Time (ms)');
            ylabel('Heart rate (/min)');
        end

        recev_data(i)=num;      %convert the num to string
        if (num>999)
            show(i,1:4)=num2str(recev_data(i));  
            flag_longVAI=1 ;    %set the flag if the VAI is in 4 digits
        elseif (i==1)
            show(i,1:3)=num2str(recev_data(i));   
            flag_longVAI=0; 
        elseif(i==2 && num<100)
             show(i,2:3)=num2str(recev_data(i));
             show(i,1)='0';
        else
            show(i,1:3)=num2str(recev_data(i));   
        end
    end 
    %show the data
    if (flag_longVAI==1)
        set(handles.VAI_text, 'String',show(1,1:4));
    else 
        set(handles.VAI_text, 'String',show(1,1:3));
    end
	set(handles.AVI_text, 'String',show(2,1:3));  
    if(heart_fast==1)
        set(handles.HR_text, 'String',show(3,1:3)); 
    else
        set(handles.HR_text, 'String',show(3,1:2));  
    end
    pause(0.1);
end 
if (request_all==0)         %to stop the pacemaker
    fwrite(t,'0');
    pause(0.1);
end

% --- Executes on button press in Set bottom.
function pb_set_Callback(hObject, eventdata, handles)
global t G_VAI G_AVI;
req_data='8';            %set VAI AVI value 
fwrite(t,req_data);
pause(0.1);

VAI_value = get(handles.VAI_text,'String');     %get the value from GUI
VAI_value=str2num(VAI_value);
G_VAI=VAI_value;
if(VAI_value<1000)
    VAI_value=VAI_value+9000;
end
VAI_value=num2str(VAI_value);

AVI_value = get(handles.AVI_text,'String');          %get the value from GUI
AVI_value=str2num(AVI_value);
G_AVI=AVI_value;
if(AVI_value<100)
    AVI_value=AVI_value+900;
end
AVI_value=num2str(AVI_value);

fwrite(t,VAI_value);
pause(0.1);
fwrite(t,AVI_value);
pause(0.1);


% --- Executes on button press in Disconnect.
function pb_disconnect_Callback(hObject, eventdata, handles)
global t;
fwrite(t,'2');
pause(0.5);
fclose(t);


% --- Executes on button press in Begin
function pb_begin_Callback(hObject, eventdata, handles)
global t;
fwrite(t,'1');          %begin 


% --- Executes on selection change in listbox1.
function listbox1_Callback(hObject, eventdata, handles)
% hObject    handle to listbox1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns listbox1 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from listbox1


% --- Executes during object creation, after setting all properties.
function listbox1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to listbox1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pushbutton13.
function pushbutton13_Callback(hObject, eventdata, handles)
% Data = cell(3,2);
% aa={'asdf','sdf','sdfsd'};
% % aa=aa';
% Data(:,1)=aa;
% % aa={aa  'abc'};
% % set(handles.listbox1,'String',aa);
% set(handles.uitable1, 'ColumnName', {'1','2'}, 'data',Data);    
% hObject    handle to pushbutton13 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on selection change in listbox2.
function listbox2_Callback(hObject, eventdata, handles)
% hObject    handle to listbox2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns listbox2 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from listbox2


% --- Executes during object creation, after setting all properties.
function listbox2_CreateFcn(hObject, eventdata, handles)
% hObject    handle to listbox2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes during object creation, after setting all properties.
function uitable1_CreateFcn(hObject, eventdata, handles)


% --- Executes on button press in pb_record.
function pb_record_Callback(hObject, eventdata, handles)
global t;
n=0;
SIZE=1024;
req_data='3';            %record command
fwrite(t,req_data);
pause(0.1);



for i=1:SIZE
    data=fread(t,4)';     
    num=str2num(char(data));
    if num>9000
        num=num-9000;
    else if num==8888
        flag=i; 
    end
    end
    VAI_record{1,i}=num2str(num);
   
end
Data(:,2)=VAI_record;

for i=1:SIZE
    data=fread(t,3)';     
    num=str2num(char(data));
    if num>900
        num=num-900;
    end
    AVI_record{1,i}=num2str(num);
end
Data(:,3)=AVI_record;

for i=1: (SIZE-flag)
    id{1,flag+i}=num2str(i);
end
for i=1:(flag-1)
    id{1,i}=num2str(i+SIZE-flag);
end
Data(:,1)=id;

set(handles.uitable2, 'ColumnName', {'id','VAI','AVI'}, 'data',Data); 
