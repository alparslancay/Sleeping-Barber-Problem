#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

sem_t waitingArea; // Bekleme Alani Semaforu
sem_t shavingArea; // Tras Semaforu
sem_t barberSleeping; // Berber Uyumsa Semaforu
sem_t barberWaiting;

pthread_mutex_t mutex;

int currentNumberCustomers = 0;

void *Barber();
void *Customer();

int currentCustomerNumber = -1;

int main()
{
    int isBarberSleeping = 1;
    int i; //Ortak for döngüleri için.
    srand(time(NULL));
    int thisDayNumberCustomers = 5 + rand()%11; // Gün boyunca 5 ile 15 arasý müþteri gelmesi isteniyor.
    printf("Müşteri sayısı: %d\n",thisDayNumberCustomers);

    pthread_t customerThreads[thisDayNumberCustomers], barberThread; // Berber ve müþteri Thread'leri tanýmlandý.
    //Semaforlar tanýmlanýyor.
    sem_init(&waitingArea,1,5); // Bekleme alanýnýn müþteri limiti 5 ayarlandý.
    sem_init(&shavingArea,1,1); // Traþ için müþteri limiti 1 ayarlandý.
    sem_init(&barberSleeping,1,0); // Berber uyuma semaforu konuldu.
    sem_init(&barberWaiting,1,0); // Berberin müşteriyi bekleme semaforu konuldu.


    //Thread'ler oluþturuluyor

    pthread_mutex_init(&mutex,NULL);

    pthread_create(&barberThread,NULL,Barber,NULL);

    for(i=0; i< thisDayNumberCustomers;i++)
        pthread_create(&customerThreads[i],NULL,Customer,(void *)i+1);

    //Join iþlemi çökme olmasýn diye create'lerden ayrý yazýlýyor.
    pthread_join(barberThread,NULL);

    for(i=0; i< thisDayNumberCustomers;i++)
        pthread_join(customerThreads[i],NULL);


    return 0;
}

void *Customer(void * customerNumber){
sleep(5 + rand()%25);

printf("\n %d.numarali Müşteri berber dükkanının önüne geldi.\n",(int)customerNumber);


sem_wait(&waitingArea);

currentNumberCustomers++;

printf("%d numaralı Müşteri bekleme alanına geçti.\n",(int)customerNumber);

if(currentNumberCustomers == 1)
{
    printf("%d numaralı müşteri berberi uyandirdi.\n",(int)customerNumber);
    sem_post(&barberSleeping);
}
sem_wait(&shavingArea);

sem_post(&waitingArea);

currentCustomerNumber = (int)customerNumber;

printf("%d numaralı Müşteri traş olma alanına geçti.\n",(int)customerNumber);

sem_post(&barberWaiting);
}

void *Barber(){
while(1){

if(currentNumberCustomers <= 0){
    printf("Barber Sleeping\n");
    sem_wait(&barberSleeping);
}

sem_wait(&barberWaiting);

printf("\nAnlık müşteri sayısı: %d\n",currentNumberCustomers);

printf("%d.Müşteri Traş oluyor.\n",currentCustomerNumber);
sleep(1);
printf("%d.Müşteri traş oldu.\n",currentCustomerNumber);

currentNumberCustomers--;

printf("%d.Müşteri dükkandan çıktı.\n",currentCustomerNumber);

sem_post(&shavingArea);


}
}
