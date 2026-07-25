int main() {
    string scanURL = "https://nadd-attendance-marker.vercel.app/scan.html?session=123we13"; /*+ newSession.sessionCode;*/

    QrCode qr = QrCode::encodeText(scanURL.c_str(), QrCode::Ecc::MEDIUM);

    int scale = 20;
    int border = 15;
    int qrcodeSize = (qr.getSize() + border * 2) * scale;
    string svg = "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"" + to_string(qrcodeSize) + "\"height=\"" + to_string(qrcodeSize) + "\">";   
           svg +="<rect width=\"100%\" height=\"100%\" fill=\"white\" />"; 
                 for(int x= 0; x <qrcodeSize; x++){
                    for(int y = 0; y <qrcodeSize; y++){
                        if(qr.getModule(x,y)){
                           svg += "<rect x=\"" + to_string((x + border) * scale) + "\" y=\"" + to_string((y + border) * scale) + 
                                    "\" width=\"" + to_string(scale) + "\" height=\"" + to_string(scale) + "\" fill=\"black\" />";  
                        }
                    }
                 }
           svg += "</svg>";
    cout << svg;
}