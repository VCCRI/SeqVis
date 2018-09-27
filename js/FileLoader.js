var FileLoader = function ( editor ) {
    

    this.ROOTTHREE = 1.7321;
    this.ROOTSIX = 2.4495;
    var scope = this;
    var signals = this.signals = editor.signals;
    this.editor = editor;
 
    this.count = 0;

    signals.fileLoaded.add(function( file, type ){
        console.log('file loaded ');
        if(type == "") scope.loadFile( file ); 
        else scope.loadDataString( file,type ); 
    });

};


FileLoader.prototype = {

    // load a zip folder
    loadFile: function( file ){
        var scope = this;
        scope.editor.clear();
        this.count += 1;

        var reader = new FileReader();
        // if (this.count > 1) return;
        reader.addEventListener( 'load', function ( event ) {	
            var contents = event.target.result;
            
            // console.log(JSON.stringify(contents));
            if(file.name.match(/\.fasta/)){
                scope.dataPreprocessFasta(contents);
                console.log('fasta');
                scope.signals.dataPrepared.dispatch();
            }
            else if(file.name.match(/\.nex/)){
                   console.log('nex');
                scope.dataPreprocessNex(contents);
                scope.signals.dataPrepared.dispatch();
            }
            else if(file.name.match(/\.phy/)){
                   console.log('phy');
                scope.dataPreprocessPhy(contents);
                scope.signals.dataPrepared.dispatch();
            }
            
        }, false );
        
        reader.readAsBinaryString( file );
    },

    loadDataString:function(contents,type){
 
        
        if(type=='fasta'){
            this.dataPreprocessFasta(contents);
            this.signals.dataPrepared.dispatch();
        }
        else if(type=='nex'){
            this.dataPreprocessNex(contents);
 
            this.signals.dataPrepared.dispatch();
        }
        else if(type=='phy'){
            this.dataPreprocessPhy(contents);
            this.signals.dataPrepared.dispatch();
        }
    },
    
    dataPreprocessFasta : function( data ){
        var words = data.split(/>/);
        for(var i=0;i<words.length;i++){
            if(words[i]=="") continue;
            var firstOccur = words[i].indexOf('\n');
            
            var seqName = words[i].substring(0, firstOccur + 1);
            var seq = words[i].substring( firstOccur+1, words[i].length);
            seqName = seqName.replace(/\s/g,"");
            seq = seq.replace(/\s/g,"");

            this.editor.inputData[seqName] = this.getSeqInfo(seq);
        
        }
        // console.log(this.editor.inputData);

        
    },
    
    dataPreprocessPhy : function(data){
        var words = data.split(/\n/);

        var seqName="";
        var seq="";
        for(var i=1; i< words.length;i++){
          var line = words[i].replace(/\r/g,"").split(/ /);
          if(line.length>1){
              if(seqName!=""){
                this.editor.inputData[seqName] = this.getSeqInfo(seq);
              }
              seqName = line[0];
              seq = "";
              for(var j =1;j<line.length;j++){
                  if(line[j]!=""){
                      seq += line[j];
                  }
              }
          }
          else{
              seq += line;
          }
                
        }

        
    },
    
    dataPreprocessNex : function(data){
        var words = data.split(/MATRIX/);
        var result = words[1].split(/\n/);

        var seq="";
        var seqName ="";
        for(var i=0; i< result.length;i++){
            result[i] = result[i].replace(/\r/,"");
            result[i] = result[i].replace(/\t/,"");
            if(result[i]!=""&&result[i]!=';'&&result[i]!="END;"){
                var line = result[i].split(/ /);
                seqName = line[0];
                for(var j=1;j<line.length;j++){
                    if(line[j]!="") seq +=line[j];
                }
                this.editor.inputData[seqName] = this.getSeqInfo(seq);
                seq="";
            }
        }  
        // this.editor.inputData = input;

    },

    getSeqInfo : function( data ) {
        //orginal
        var allPositionFreq={};
        allPositionFreq['A']=0;
        allPositionFreq['C']=0;
        allPositionFreq['G']=0;
        allPositionFreq['T']=0;
     
        
        //first position
        var firstPositionFreq ={};
        firstPositionFreq['A'] = 0;
        firstPositionFreq['C'] = 0;
        firstPositionFreq['G'] = 0;
        firstPositionFreq['T'] = 0;
    
        //second position
        var secondPositionFreq ={};
        secondPositionFreq['A'] = 0;
        secondPositionFreq['C'] = 0;
        secondPositionFreq['G'] = 0;
        secondPositionFreq['T'] = 0;
   
        //third position
        var thirdPositionFreq ={};
        thirdPositionFreq['A'] = 0;
        thirdPositionFreq['C'] = 0;
        thirdPositionFreq['G'] = 0;
        thirdPositionFreq['T'] = 0;
     

        var firstPositionLen = 0;
        var secondPositionLen = 0;
        var thirdPositionLen = 0;
        var unknownSeqNum = 0;
        var otherSeqNum = 0;
        var gapNum = 0;
        var weight; 
        for(var i=0;i<data.length;i++){
            currCode = data[i];
            // if (/^[A-Z]$/i.test(currCode)==false) continue;
        
            //allPositionFreq[currCode]++;
            if( currCode == 'N' ){
                unknownSeqNum += 1;
                continue;
            }
            if( currCode == ' '){
                gapNum += 1;
                continue;
            }
            else if( currCode == 'A' || currCode == 'G' || currCode == 'C'|| currCode == 'T' ){
                //TODO gap also counts to total length?
                allPositionFreq[currCode]++;
          
                if(i%3==0){
                    firstPositionLen += 1; 
                    firstPositionFreq[currCode]++;
                }
                else if(i%3==1){
                    secondPositionLen += 1; 
                    secondPositionFreq[currCode]++;
                }
                else if(i%3==2){
                    thirdPositionLen += 1; 
                    thirdPositionFreq[currCode]++;
                }
                 
            }
            else {
                otherSeqNum += 1;
                // allPositionFreq['A']+=0.25;
                // allPositionFreq['G']+=0.25;
                // allPositionFreq['C']+=0.25;
                // allPositionFreq['T']+=0.25;
                if(i%3==0){
                    firstPositionLen += 1; 
                    // firstPositionFreq['A']+=0.25;
                    // firstPositionFreq['G']+=0.25;
                    // firstPositionFreq['C']+=0.25;
                    // firstPositionFreq['T']+=0.25;
                }
                else if(i%3==1){
                    secondPositionLen += 1; 
                    // secondPositionFreq['A']+=0.25;
                    // secondPositionFreq['G']+=0.25;
                    // secondPositionFreq['C']+=0.25;
                    // secondPositionFreq['T']+=0.25;
                }
                else if(i%3==2){
                    thirdPositionLen += 1; 
                    // thirdPositionFreq['A']+=0.25;
                    // thirdPositionFreq['G']+=0.25;
                    // thirdPositionFreq['C']+=0.25;
                    // thirdPositionFreq['T']+=0.25;               
                }

             }

        }
        // console.log(data.length)
        var allPositionLengh = data.length-unknownSeqNum;
        allPositionFreq['A'] /= allPositionLengh;
        allPositionFreq['C'] /= allPositionLengh;
        allPositionFreq['G'] /= allPositionLengh;
        allPositionFreq['T'] /= allPositionLengh;

        firstPositionFreq['A'] /= firstPositionLen;
        firstPositionFreq['C'] /= firstPositionLen;
        firstPositionFreq['G'] /= firstPositionLen;
        firstPositionFreq['T'] /= firstPositionLen;

        secondPositionFreq['A'] /= secondPositionLen;
        secondPositionFreq['C'] /= secondPositionLen;
        secondPositionFreq['G'] /= secondPositionLen;
        secondPositionFreq['T'] /= secondPositionLen;

        thirdPositionFreq['A'] /= thirdPositionLen;
        thirdPositionFreq['C'] /= thirdPositionLen;
        thirdPositionFreq['G'] /= thirdPositionLen;
        thirdPositionFreq['T'] /= thirdPositionLen;


        return {
                allPositionFreq:allPositionFreq,
                firstPositionFreq:firstPositionFreq,
                secondPositionFreq:secondPositionFreq,
                thirdPositionFreq:thirdPositionFreq,
                unknownSeqNum:unknownSeqNum,
                gapNum:gapNum,
                otherSeqNum:otherSeqNum,
                totalLen:data.length,
                sequence:data
                };
    },
    


};