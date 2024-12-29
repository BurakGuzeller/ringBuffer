/*
 * ringBuffer.c
 *
 *  Created on: Dec 25, 2024
 *      Author: burakguzeller
 */

#include "ringBuffer.h"


/*
 * @brief  : Verilen ring buffer'ı sıfırlar.
 * @param  : pRingBuffer - Sıfırlanacak ring buffer nesnesinin pointer'ı.
 */
void resetBuffer(Ring_Buffer_t *pRingBuffer) {
    pRingBuffer->head = 0;
    pRingBuffer->tail = 0;
    memset(pRingBuffer->buffer, 0, RING_MAX_SIZE);
}


/*
 * @brief  : Ring buffer'ın boş olup olmadığını kontrol eder.
 * @param  : pRingBuffer - Kontrol edilecek ring buffer nesnesi.
 * @return : true - Buffer boş, false - Buffer dolu.
 */
bool bufferIsEmpty(Ring_Buffer_t *pRingBuffer) {
    return (pRingBuffer->head == pRingBuffer->tail);
}

/*
 * @brief  : Ring buffer'ın dolu olup olmadığını kontrol eder.
 * @param  : pRingBuffer - Kontrol edilecek ring buffer nesnesi.
 * @return : true - Buffer dolu, false - Buffer dolu değil.
 */
bool bufferIsFull(Ring_Buffer_t *pRingBuffer) {
    return ((pRingBuffer->head + 1) % RING_MAX_SIZE == pRingBuffer->tail);
}


/*
 * @brief  : Ring buffer'a veri yazar. head günceller
 * @param  : pRingBuffer - Buffer nesnesi, data - Yazılacak veri.
 * @return : true - Veri yazıldı, false - Buffer dolu.
 */
void writeBufferData(Ring_Buffer_t *pRingBuffer, uint8_t data) {
    if (bufferIsFull(pRingBuffer)) {
        pRingBuffer->tail = (pRingBuffer->tail + 1) % RING_MAX_SIZE;
    }
    pRingBuffer->buffer[pRingBuffer->head] = data;
    pRingBuffer->head = (pRingBuffer->head + 1) % RING_MAX_SIZE;
}


/*
 * @brief  : Ring buffer'dan veri okur. tail günceller
 * @param  : pRingBuffer - Buffer nesnesi.
 * @return : Okunan veri (buffer boşsa 0 döner).
 */
uint8_t readBufferData(Ring_Buffer_t *pRingBuffer) {
    uint8_t data = 0;
    if (!bufferIsEmpty(pRingBuffer)) {
        data = pRingBuffer->buffer[pRingBuffer->tail];
        pRingBuffer->tail = (pRingBuffer->tail + 1) % RING_MAX_SIZE;
    }
    return data;
}


/*
 * @brief  : Ring buffer içinde belirli bir string'i arar. tail guncellenmez sadece olup olmadıgı hakkında bilgi alinir.
 * @param  : pRingBuffer - Buffer nesnesi, str - Aranacak string.
 * @return : true - String bulundu, false - String bulunamadı.
 */
bool findDataInBuffer(Ring_Buffer_t *pRingBuffer, const char *str) {
    uint16_t strIndex = 0;
    uint16_t strSize = strlen(str);
    uint16_t length = (pRingBuffer->head >= pRingBuffer->tail)
                      ? (pRingBuffer->head - pRingBuffer->tail)
                      : (RING_MAX_SIZE - pRingBuffer->tail + pRingBuffer->head);

    if (strSize > RING_MAX_SIZE || bufferIsEmpty(pRingBuffer)) {
        return false;
    }

    for (uint16_t i = 0; i < length; i++) {
        uint16_t currentIndex = (pRingBuffer->tail + i) % RING_MAX_SIZE;
        if (pRingBuffer->buffer[currentIndex] == str[strIndex]) {
            strIndex++;
            if (strIndex == strSize) {
                pRingBuffer->tail = (pRingBuffer->tail + strIndex) % RING_MAX_SIZE;
                return true;
            }
        } else {
            strIndex = 0;
        }
    }

    return false;
}



/*
 * @brief  : Ring buffer'dan belirtilen veriyi ve sonrakini kopyalar.
 * @param  : pRingBuffer - Buffer nesnesi, copyField - Verinin kopyalanacağı alan, copy - Aranan string.
 */
void copyBufferData(Ring_Buffer_t *pRingBuffer, char copyField[], uint8_t fieldSize, const char *copy) {
    memset(copyField, 0, fieldSize); // Çıkış dizisini sıfırla

    if (bufferIsEmpty(pRingBuffer)) {
        return;
    }

    char *ret = strstr((char *)pRingBuffer->buffer, copy);
    if (ret != NULL) {
        size_t retLen = strlen(ret);

        uint8_t tempBuffer[retLen + 1];
        memset(tempBuffer, 0, sizeof(tempBuffer));
        memcpy(tempBuffer, ret, retLen);

        for (int i = 0; i < retLen && i < fieldSize - 1; i++) {
            copyField[i] = tempBuffer[i];
            if (tempBuffer[i] == '\0' || tempBuffer[i] == '\r' || tempBuffer[i] == '\n') {
                copyField[i] = '\0'; // String sonlandır
                break;
            }
        }
    }
}


/**
 * @brief  Bir ring buffer'dan belirli bir desenden sonra, belirtilen bitiş karakterine kadar olan veriyi kopyalar.
 *
 * Bu fonksiyon, bir ring buffer içinde belirli bir başlangıç desenini arar. Eğer bulunursa,
 * desenden sonraki tüm karakterleri, belirtilen bitiş karakterine veya tamponun sonuna
 * ulaşılana kadar verilen hedef alana (output) kopyalar.
 *
 * @param pRingBuffer:  Verileri içeren ring buffer yapısına işaretçi.
 * @param copyField : Çıkarılan verilerin saklanacağı hedef dizi.
 * @param copyStart : Bufferdaki başlangıç desenidir. Bu desenden sonraki karakterden itibaren veri kopyalanır.
 * @param copyFinish : Kopyalanacak verinin sonunu belirten bitiş karakteridir.
 * @param len        : Kopyalanacak maksimum bayt sayısı
 *
 */
void copyAfterPattern(Ring_Buffer_t *pRingBuffer, char copyField[], const char *copyStart, char copyFinish, uint16_t len) {
    uint8_t tempBuffer[len];
    uint16_t tempIndex = 0;
    char *ret = strstr((char *)pRingBuffer->buffer, copyStart);

    if (ret != NULL) {
        ret += strlen(copyStart);
        char ch = *ret;
        while (tempIndex < len - 1) {
            if (ch == copyFinish || ch == '\0') {
                break;
            }
            tempBuffer[tempIndex++] = ch;
            ret++;
            ch = *ret;
        }
        tempBuffer[tempIndex] = '\0';
        memcpy(copyField, tempBuffer, tempIndex + 1);
    }
    else
    {
        copyField[0] = '\0';
    }
}

/*
 * @brief  : Ring buffer'dan veriyi belirtilen sayıda kopyalar.
 * @param  : pRingBuffer - Buffer nesnesi, dest - Verinin kopyalanacağı hedef alan, count - Kopyalanacak veri miktarı.
 */
void copyCircularBuffer(Ring_Buffer_t *pRingBuffer, uint8_t *dest, uint16_t count) {
    for (uint16_t i = 0; i < count; i++) {
        dest[i] = readBufferData(pRingBuffer);
    }
}
