//===----------------------------------------------------------------------===//
// Lexer
// このセクションでは、Lexerというテキストから「数字」や「演算子」をまとまり毎に
// 分割するクラスを実装している。
//
// 全体的な流れとしては、gettokをParserから呼ぶことにより「次のトークン」を読み、
// それが数値リテラルだった場合はnumValという変数にセットする。
//===----------------------------------------------------------------------===//
#include <regex>

// このLexerでは、EOF、数値、"def"、識別子以外は[0-255]を返す('+'や'-'を含む)。
enum Token {
    tok_eof = -1,
    tok_def = -2,
    tok_identifier = -3,
    tok_number = -4,
    tok_if = -5,
    tok_then = -6,
    tok_else = -7,
    tok_sge = -8,
    tok_sle = -9
};

class Lexer {
    public:
        // gettok - トークンが数値だった場合はnumValにその数値をセットした上でtok_number
        // を返し、トークンが識別子だった場合はidentifierStrにその文字をセットした上でtok_identifierを返す。
        // '+'や他のunknown tokenだった場合はそのascii codeを返す。
        int gettok() {
            static int lastChar = getNextChar(iFile);
            static int lastTok = tok_eof;
            // スペースをスキップ
            std::cout << "Cur is:"<<(char)lastChar << std::endl;
            while (isspace(lastChar))
                lastChar = getNextChar(iFile);

            // TODO 2.1: 識別子をトークナイズする
            // 1.3と同様に、今読んでいる文字がアルファベットだった場合はアルファベットで
            // なくなるまで読み込み、その値をidentifierStrにセットする。
            // 読み込んだ文字が"def"だった場合は関数定義であるためtok_defをreturnし、
            // そうでなければ引数の参照か関数呼び出しであるためtok_identifierをreturnする。
            if (isalpha(lastChar)) {
                identifierStr = lastChar;
                while (isalnum((lastChar = getNextChar(iFile))))
                    identifierStr += lastChar;

                if (identifierStr == "def")
                    return setTok(tok_def,lastTok);
                // TODO 3.2: "if", "then", "else"をトークナイズしてみよう
                // 上記の"def"の例を参考に、3つの予約語をトークナイズして下さい。
                if (identifierStr == "if")
                    return setTok(tok_if,lastTok);
                if (identifierStr == "then")
                    return setTok(tok_then,lastTok);
                if (identifierStr == "else")
                    return setTok(tok_else,lastTok);
                return setTok(tok_identifier,lastTok);
            }

            // TODO 1.3: 数字のパーシングを実装してみよう
            // 今読んでいる文字(lastChar)が数字だった場合(isdigit(lastChar) == true)は、
            // 数字が終わるまで読み、その数値をnumValにセットする。
            // 1. lastCharが数字かチェックする e.g. if (isdigit(lastChar)) {..
            // 2. 数字を格納するための変数を作る e.g. std::string str = "";
            // 3. 入力が'456'という数字だったとして、今の文字は'4'なので、
            //    次の文字が数字ではなくなるまで読み込み、2で作った変数に格納する
            //    e.g. str += lastChar;
            // 4. 次の文字が数字でなくなるまで読み込む。ファイルから次の文字を読み込むのは、
            //    getNextChar(iFile)で出来る。 e.g. while(isdigit(lastChar = getNextChar(iFile))) {..
            // 5. 2と同様に、4が数字であれば2で作った変数に格納する
            // 6. 次の文字が数字でなくなったら、setnumValを読んでnumValに数値を格納する。その際、
            //   strtodというcharからintにする関数が利用できる。
            // 7. このトークンは数値だったので、tok_numberをreturnする。
            //
            // ここに実装して下さい
            if (isdigit(lastChar)) {
                return setTok(parseNum2(iFile,lastChar), lastTok);

            }else if(lastChar == '-'){
                if(lastTok == tok_identifier || lastTok == tok_number){
                    lastChar = getNextChar(iFile);
                    return setTok('-',lastTok);
                    //この場合は'-'は演算子
                }
                lastChar = getNextChar(iFile);
                return setTok(parseNum2(iFile,lastChar,'-'), lastTok);
            }


            // TODO 1.4: コメントアウトを実装してみよう
            // '#'を読んだら、その行の末尾まで無視をするコメントアウトを実装する。
            // 1. 今の文字(LastChar)が'#'かどうかをチェック
            // 2. lastCharに次のトークンを読み込む(getNextChar(iFile)を使う)
            // 3. lastCharが'\n'か、EOFになるまで読み込む。e.g. whi2le(lastChar != EOF && lastChar != '\n')
            // 4. lastCharがEOFでない場合、行末まで読み込んだので次のトークンに進むためにgettok()をreturnする。
            //
            // ここに実装して下さい
            if (lastChar == '#') {
                // Comment until end of line.
                lastChar = getNextChar(iFile);
                while (lastChar != EOF && lastChar != '\n')
                    lastChar = getNextChar(iFile);

                if (lastChar != EOF)
                    return gettok();
            }

            // EOFならtok_eofを返す
            if (iFile.eof())
                return setTok(tok_eof,lastTok);

            // tok_numberでもtok_eofでもなければそのcharのasciiを返す
            int thisChar = lastChar;
            lastChar = getNextChar(iFile);
            std::string op;
            op = (char)thisChar;
            op += lastChar;
            if(op=="<="){
                lastChar = getNextChar(iFile);
                return setTok(tok_sle,lastTok);
            }
            if(op==">="){
                lastChar = getNextChar(iFile);
                return setTok(tok_sge,lastTok);
            }
            std::cout << "Not <= :" <<(char)lastChar << std::endl;
            return setTok(thisChar,lastTok);
        }

        // 数字を格納するnumValのgetter, setter
        uint64_t getNumVal() { return numVal; }
        void setnumVal(uint64_t numval) { numVal = numval; }

        // 識別子を格納するIdentifierStrのgetter, setter
        std::string getIdentifier() { return identifierStr; }
        void setIdentifier(std::string str) { identifierStr = str; }

        void initStream(std::string fileName) { iFile.open(fileName); }

            private:
        std::ifstream iFile;
        uint64_t numVal;
        // tok_identifierなら文字を入れる
        std::string identifierStr;
        static char getNextChar(std::ifstream &is) {
            char c = '\0';
            if (is.good())
                is.get(c);

            return c;
        }
        
        int parseNum(std::ifstream &is,int &lc,char sign = '+'){
                std::string numStr = "";
                numStr += sign;
                numStr += lc;
                while (isdigit(lc = getNextChar(is))){
                    numStr += lc;
                }
                if(lc=='.'){
                    numStr+=lc;
                    lc = getNextChar(is);
                }
                while (isdigit(lc)){
                    numStr += lc;
                    lc = getNextChar(is);
                }
                if(lc=='e' || lc=='E'){
                    numStr+=lc;
                    lc = getNextChar(is);   
                    if(lc=='+' || lc=='-'){
                        numStr+=lc;
                        lc = getNextChar(is);
                    }
                    while (isdigit(lc)){
                        numStr += lc;
                        lc = getNextChar(is);
                    }
                }
                //std::cout<< "Parse Num :" << numStr << std::endl;
                setnumVal(strtod(numStr.c_str(), nullptr));
                return tok_number;
            }
            int parseNum2(std::ifstream &is,int &lc,char sign = '+'){
                std::string numStr = "";
                numStr += sign;
                while (true){
                    numStr +=lc;
                    lc = getNextChar(is);if(!isdigit(lc) && lc!='.' && lc!='e' && lc!='E'){
                        if(isdigit(numStr.back())){
                            break;
                        }
                    }
                }
                std::regex re(R"([+|-]?[0-9]+(\.[0-9]+)?([e|E][+|-]?[0-9]+)?)");
                std::smatch match;
                if(std::regex_match(numStr,match,re)){
                    setnumVal(strtod(numStr.c_str(), nullptr));
                    //std::cout<< "Parse Num :" << numStr << std::endl;
                    return tok_number;
                }
                return lc;
            }

            int setTok(int NextTok, int &lT){
                lT = NextTok;
                return NextTok;
            }
        };
        
