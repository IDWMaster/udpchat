#include "cppext.h"


int main(int argc, char** argv) {
  std::shared_ptr<System::Net::UDPSocket> s;
  
    s = System::Net::CreateUDPSocket();
    
  System::Net::IPEndpoint remoteep;
  if(argc>1) {
    //Connect to server
    System::Net::IPEndpoint mep;
    mep.ip = argv[1];
    mep.port = atoi(argv[2]);
    remoteep = mep;
  }else {
    //Run server
    s = System::Net::CreateUDPSocket();
    System::Net::IPEndpoint ep;
    s->GetLocalEndpoint(ep);
    printf("Running server on port %i\n",ep.port);
  }
  
  char mander[1024];
  std::shared_ptr<System::Net::UDPCallback> cb = System::Net::F2UDPCB([&](System::Net::UDPCallback& results){
    mander[results.outlen] = 0; //NULL-terminate string
    printf("%s",mander);
    remoteep = results.receivedFrom;
    s->Receive(mander,1024,cb);
  });
  s->Receive(mander,1024,cb);
  
  char inbuffer[1024];
  std::shared_ptr<System::IO::Stream> stdin = System::IO::FD2S(0);
  std::shared_ptr<System::IO::IOCallback> inputcb = System::IO::IOCB([&](System::IO::IOCallback& results){
    s->Send(inbuffer,results.outlen,remoteep);
    stdin->Read(inbuffer,1024,inputcb);
  });
  stdin->Read(inbuffer,1024,inputcb);
  System::Enter();
}