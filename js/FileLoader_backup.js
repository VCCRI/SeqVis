var FileLoader = function ( editor ) {
    
    this.FLOWFILENAME  = 'flows.csv';
    this.EDGEFILENAME = 'networks.csv';
    this.NODEFILENAME = 'nodes.csv'; 
    this.TRIPFILENAME = 'trips.csv';
    this.ROOTTHREE = 1.7321;
    this.ROOTSIX = 2.4495;
    var scope = this;
    var signals = this.signals = editor.signals;
    this.editor = editor;
    var reader = this.reader = new FileReader();


    signals.fileLoaded.add(function( file ){
        scope.loadFile( file ); 
    });

};


FileLoader.prototype = {

    // load a zip folder
    loadFile: function( file ){
        var scope = this;
        scope.reader.addEventListener( 'load', function ( event ) {	
            
            var contents = event.target.result;
            scope.dataPreprocessFasta(contents);
            
        }, false );
        
        scope.reader.readAsText( file );
    },
    
    
    
    dataPreprocessFasta : function( data ){
        var total = 0;
        var words = data.split(/>/);
        var input = {
            single:{},
            first:{},
            second:{},
            third:{},

            rawData:{},
            singleSeq:{},
            firstSeq:{},
            secondSeq:{},
            thirdSeq:{}
        };
        
        for(var i=0;i<words.length;i++){
            if(words[i]=="") continue;
            var firstOccur = words[i].indexOf('\n');
            
            var seqName = words[i].substring(0, firstOccur + 1);
            var seq = words[i].substring( firstOccur+1, words[i].length);
            seqName = seqName.replace(/\s/g,"");
            seq = seq.replace(/\s/g,"");
    
            // console.log(JSON.stringify(seq));
            currFreqs = this.getFrequency(seq);
            input.single[seqName] = currFreqs[0];
            input.first[seqName] = currFreqs[1];
            input.second[seqName] = currFreqs[2];
            input.third[seqName] = currFreqs[3];

            input.rawData[seqName] = seq;
            input.singleSeq[seqName] = currFreqs[4];
            input.firstSeq[seqName] = currFreqs[5];
            input.secondSeq[seqName] = currFreqs[6];
            input.thirdSeq[seqName] = currFreqs[7];
        }
    
        
        this.editor.inputData = input;
        this.signals.dataPrepared.dispatch();
        
    },
    
    

    getFrequency : function( data ) {
        //orginal
        var fre={};
        fre['A']=0;
        fre['C']=0;
        fre['G']=0;
        fre['T']=0;
        freSeq = '';
        
        //first position
        var fir ={};
        fir['A'] = 0;
        fir['C'] = 0;
        fir['G'] = 0;
        fir['T'] = 0;
        firstSeq = '';
        //second position
        var sec ={};
        sec['A'] = 0;
        sec['C'] = 0;
        sec['G'] = 0;
        sec['T'] = 0;
        secondSeq = '';
        //third position
        var thir ={};
        thir['A'] = 0;
        thir['C'] = 0;
        thir['G'] = 0;
        thir['T'] = 0;
        thirdSeq = '';

        var firstLen = 0;
        var secondLen = 0;
        var thirdLen = 0; 
        for(var i=0;i<data.length;i++){
            currCode = data[i];
            if (/^[A-Z]$/i.test(currCode)==false || currCode == 'N') continue;
            fre[currCode]++;
            freSeq += currCode;
            if(i%3==0){
                firstLen += 1; 
                fir[currCode]++;
                firstSeq += currCode;
            }
            else if(i%3==1){
                secondLen += 1; 
                sec[currCode]++;
                secondSeq += currCode;
            }
            else if(i%3==2){
                thirdLen += 1; 
                thir[currCode]++;
                thirdSeq += currCode;
             }
        }
        // console.log(data.length)
        var totalLen = firstLen+secondLen+thirdLen;
        fre['A'] /= totalLen;
        fre['C'] /= totalLen;
        fre['G'] /= totalLen;
        fre['T'] /= totalLen;


        fir['A'] /= firstLen;
        fir['C'] /= firstLen;
        fir['G'] /= firstLen;
        fir['T'] /= firstLen;

        sec['A'] /= secondLen;
        sec['C'] /= secondLen;
        sec['G'] /= secondLen;
        sec['T'] /= secondLen;

        thir['A'] /= thirdLen;
        thir['C'] /= thirdLen;
        thir['G'] /= thirdLen;
        thir['T'] /= thirdLen;
        // console.log([fre,fir,sec,thir])
        return [fre,fir,sec,thir,freSeq,firstSeq,secondSeq,thirdSeq];
    },
    


};


