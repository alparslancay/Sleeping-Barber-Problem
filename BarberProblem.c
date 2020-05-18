#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

sem_t waitingArea; // Bekleme Alani Semaforu
sem_t shaving; // Tras Semaforu
sem_t barberSleeping; // Berber Uyumsa Semaforu

int currentNumberCustomer = 0;

void *Barber();
void *Customer();

int main()
{
    int isBarberSleeping = 1;
    int i; //Ortak for döngüleri için.
    srand(time(NULL));
    int thisDayNumberCustomers = 5 + rand()%35; // Gün boyunca 5 ile 35 arasý müþteri gelmesi isteniyor.

    pthread_t customerThreads[thisDayNumberCustomers], barberThread; // Berber ve müþteri Thread'leri tanýmlandý.
    //Semaforlar tanýmlanýyor.
    sem_init(&waitingArea,1,5); // Bekleme alanýnýn müþteri limiti 5 ayarlandý.
    sem_init(&shaving,1,1); // Traþ için müþteri limiti 1 ayarlandý.
    sem_init(&barberSleeping,1,0); // Berber uyuma semaforu konuldu.


    pthread_create(&barberThread,NULL,Barber,NULL);

    //Thread'ler oluþturuluyor
    for(i=0; i< thisDayNumberCustomers;i++)
        pthread_create(&customerThreads[i],NULL,Customer,NULL);

    //Join iþlemi çökme olmasýn diye create'lerden ayrý yazýlýyor.
    pthread_join(barberThread,NULL);

    for(i=0; i< thisDayNumberCustomers;i++)
        pthread_create(&customerThreads[i],NULL);


    return 0;
}

void *Customer(){
sleep(1);

printf("\n Müþteri berber dükkanýnýn önüne geldi.\n");

sem_wait(waitingArea);

currentNumberCustomer++;

printf("Müþteri bekleme alanýna geçti.");

if(currentNumberCustomer == 1)
    sem_post(barberSleeping);
sem_wait(shaving);

printf("Müþteri traþ olma alanýna geçti.");

}

void *Barber(){

if(currentNumberCustomer == 0)
    sem_wait(barberSleeping);

sem_post(shaving);
printf("Müþteri traþ oldu.");

sem_post(waitingArea);

currentNumberCustomer--;

}
