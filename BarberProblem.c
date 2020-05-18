#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

sem_t waitingArea; // Bekleme alanı Semaforu
sem_t shavingArea; // Traş koltuğu Semaforu
sem_t barberSleeping; // Berber uyuma Semaforu
sem_t barberWaiting; //Müşteri bekleme semaforu

int currentNumberCustomers = 0; //Anlık müşteri 

void *Barber();
void *Customer();

int currentCustomerNumber = -1; //Müşteri sıra numarası

int main()
{
    int i; 
    srand(time(NULL));
    int thisDayNumberCustomers = 5 + rand()%11; // Gün boyunca 5 ile 15 arası müşteri gelmesi isteniyor.
    printf("Müşteri sayısı: %d\n",thisDayNumberCustomers);

    pthread_t customerThreads[thisDayNumberCustomers], barberThread; // Berber ve müşteri Thread'leri tanımlandı.
    //Semaforlar tanımlanıyor.
    sem_init(&waitingArea,1,5); // Bekleme alanının müşteri limiti 5 ayarlandı.
    sem_init(&shavingArea,1,1); // Traş alanının müşteri limiti 1 ayarlandı.
    sem_init(&barberSleeping,1,0); // Berber uyuma semaforu ayarlandı.
    sem_init(&barberWaiting,1,0); // Berberin müşteriyi bekleme semaforu ayarlandı.

    //Thread'ler oluşturuluyor
    pthread_create(&barberThread,NULL,Barber,NULL);

    for(i=0; i< thisDayNumberCustomers;i++)
        pthread_create(&customerThreads[i],NULL,Customer,(void *)i+1);

    //Join işlemi çökme olmasın diye create'lerden ayrı yazılıyor.
    pthread_join(barberThread,NULL);

    for(i=0; i< thisDayNumberCustomers;i++)
        pthread_join(customerThreads[i],NULL);


    return 0;
}

void *Customer(void * customerNumber){
sleep(5 + rand()%25);// Müşterilerin dükkana gelme zaman aralığı

printf("\n %d.numarali Müşteri berber dükkanının önüne geldi.\n",(int)customerNumber);

sem_wait(&waitingArea);// Müşteri bekleme alanı için sıraya giriyor.

currentNumberCustomers++;// Girdikten sonra  mevcut müşteri sayısı 1 arttırılıyor.

printf("%d numaralı Müşteri bekleme alanına geçti.\n",(int)customerNumber);

if(currentNumberCustomers == 1) // Eğer ilk müşteri ise
{
    printf("%d numaralı müşteri berberi uyandirdi.\n",(int)customerNumber);
    sem_post(&barberSleeping);//Berber uyandırılıyor
}
sem_wait(&shavingArea);// Müşteri traş alanı için sıraya giriyor.

sem_post(&waitingArea);// Müşteri bekleme alanından çıkıyor.

currentCustomerNumber = (int)customerNumber;

printf("%d numaralı Müşteri traş olma alanına geçti.\n",(int)customerNumber);

sem_post(&barberWaiting);// Müşteri traş için hazır.
}

void *Barber(){
while(1){ 

if(currentNumberCustomers <= 0){//Eğer müşteri kalmadıysa
    printf("Barber Sleeping\n");
    sem_wait(&barberSleeping);//Berber uyutulur.
}

sem_wait(&barberWaiting);// Berber traş için hazır.

printf("\nAnlık müşteri sayısı: %d\n",currentNumberCustomers);

printf("%d.Müşteri Traş oluyor.\n",currentCustomerNumber);
sleep(1);
printf("%d.Müşteri traş oldu.\n",currentCustomerNumber);

currentNumberCustomers--;// Anlık müşteri sayısı 1 azaltılıyor.

printf("%d.Müşteri dükkandan çıktı.\n",currentCustomerNumber);

sem_post(&shavingArea);// Müşteri traş koltuğundan ayrılıyor.


}
}
