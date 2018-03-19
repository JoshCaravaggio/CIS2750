$(document).ready(function(){

    addToStatusPanel("GEDCOM parser application by Joshua Caravaggio");

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/getFiles',
        success : function(data){

            var fileCounter = 0;

            for(var i in data){
                for(var j in data[i]){

                    $.ajax({
                        type: 'get',
                        dataType: 'json',
                        url: '/getFileInfo',
                        data: {
                            'fileName': data[i][j]
                        },
                        success: function(newData){

                            fileInfo = JSON.parse(newData.fileInfo);

                            if(fileInfo.fileName !="INVALID_FILE"){

                                addFileToSelect(fileInfo.fileName);
                                fileCounter++;
                                addGEDCOMtoFileLog(fileInfo, fileCounter);
                                addToStatusPanel("Loaded " + fileInfo.fileName);    

                            }

                        },
                        fail: function(newData){

                            console.log("Failure");
   
                        }
                    });
                    

                }
            }

        }, fail: function(data){

            console.log("Request broken");
        }
    });

    $('#createGEDCOM').click(function(){

        let createForm = document.getElementById("createGEDCOMForm");
        let createFileName = createForm.createFileName.value;
        let createSubName = createForm.createSubName.value;
        let createSubAddr = createForm.createSubAddr.value;               
    

        if(createFileName != "" && createSubName != "" && createSubAddr != ""){

            let newGEDCOM = {
            "source":"GEDCOM Web App",
            "gedcVersion":"5.5",
            "encoding":"ASCII",
            "subName":createSubName,
            "subAddress":createSubAddr
            }        

            let newGEDCOMdata = JSON.stringify(newGEDCOM);


            $.ajax({

                type: 'get',
                dataType: 'json',
                url: '/createGEDCOM',
                data: {
                    newGEDCOM: newGEDCOMdata,
                    fileName: createFileName
                },
                success: function(newData){

                    response = newData.response;


                    if(response=="OK"){

                        fileInfo = JSON.parse(newData.GEDCOMobject);
                        let rowCount = document.getElementById("fileLogTable").rows.length;
                        addToStatusPanel("Successfully wrote " + createFileName + " to the server");
                        addFileToSelect(createFileName);
                        addGEDCOMtoFileLog(fileInfo, rowCount);  

                    }else{
                        addToStatusPanel("Failed to write " + createFileName + " to server");

                    }                 
                }

            });
        }else{
            addToStatusPanel("Create GEDCOM error: fields cannot be empty");
        }   


    });    

    $('#addIndividual').click(function(){

        let createForm = document.getElementById("addIndividualForm");
        let givenName = createForm.givenName.value;
        let surname = createForm.surname.value;               
        let fileName = createForm.addIndFileSelect.value;

        if(givenName == "" && surname == ""){
            addToStatusPanel("Cannot add individual: atleast one name field must be present.");

        }else{
        $.ajax({

            type: 'get',
            dataType: 'json',
            url: '/addIndividual',
            data: {
                givenName: givenName,
                surname: surname,
                fileName: fileName
            },
            success: function(newData){

                response = newData.response;

            }

        });

        }


    });    

});


$(document).on('change', '#GEDViewSelect',function(e){

    let select = e.target;
    let fileName = select.value;
   // / console.log(fileName);

    $.ajax({

        type: 'get',
        dataType: 'json',
        url: '/gedView',
        data: {
            fileName: fileName
        },
        success: function(newData){

        let indData = JSON.parse(newData.data);

            for(var i in  indData){

                let individual = indData[i];

                addIndividualToGedView(indData[i], eval(i)+eval(1));          

            }
        }


    });
    
});


function addIndividualToGedView(individualData, tableRow){

    let gedViewTable = document.getElementById("gedViewTable");
    var newRow   = gedViewTable.insertRow(tableRow);

    var givenNameCell = newRow.insertCell(0);
    givenNameCell.innerHTML = individualData.givenName 

    var surnameCell = newRow.insertCell(1);
    surnameCell.innerHTML = individualData.surname;

    var sexCell = newRow.insertCell(2);
    sexCell.innerHTML = individualData.sex;


}

function addGEDCOMtoFileLog(fileInfo, tableRow){

    let fileLogTable = document.getElementById("fileLogTable");
    var newRow   = fileLogTable.insertRow(tableRow);

    var fileNameCell = newRow.insertCell(0);
    fileNameCell.innerHTML = "<a href = \"/uploads/" + fileInfo.fileName +"\">" + fileInfo.fileName + "</a>";

    var sourceCell = newRow.insertCell(1);
    sourceCell.innerHTML = fileInfo.source;

    var versionCell = newRow.insertCell(2);
    versionCell.innerHTML = fileInfo.version;

    var encodingCell = newRow.insertCell(3);
    encodingCell.innerHTML = fileInfo.encoding;

    var submitterNameCell = newRow.insertCell(4);
    submitterNameCell.innerHTML = fileInfo.submitterName;            

    var submitterAddrCell = newRow.insertCell(5);
    submitterAddrCell.innerHTML = fileInfo.submitterAddress;    

    var numIndividualsCell = newRow.insertCell(6);
    numIndividualsCell.innerHTML = fileInfo.numIndividuals;    

    var numFamiliesCell = newRow.insertCell(7);
    numFamiliesCell.innerHTML = fileInfo.numFamilies;    
    
}
function addFileToSelect(file){

    let getAnsFileSelect = document.getElementById("getAnsFileSelect");
    let getAnsOption = document.createElement("option");
    getAnsOption.text = file;
    getAnsFileSelect.add(getAnsOption);

    let getDesFileSelect = document.getElementById("getDesFileSelect");
    let getDesOption = document.createElement("option");
    getDesOption.text = file;
    getDesFileSelect.add(getDesOption);

    let addIndFileSelect = document.getElementById("addIndFileSelect");
    let addIndOption = document.createElement("option");
    addIndOption.text = file;
    addIndFileSelect.add(addIndOption);

    let gedViewFileSelect = document.getElementById("gedViewFileSelect");
    let gedViewOption = document.createElement("option");
    gedViewOption.text = file;
    gedViewFileSelect.add(gedViewOption);
}

function addToStatusPanel(statusString){

    let statusTable = document.getElementById("statusTable");
    let rowCount = statusTable.rows.length;
    let newRow   = statusTable.insertRow(rowCount);
    let infoCell = newRow.insertCell(0);
    infoCell.innerHTML = "> " + statusString + "<br>"

}