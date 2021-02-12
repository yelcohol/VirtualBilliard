#include "sound.h"

//�Լ��� : CreateDirectSound() 
//����   : DirectSound ��ü�� �����ϰ� ���·����� �����Ѵ�.
BOOL Sound::CreateDirectSound(HWND hWnd)
{
	//���̷�Ʈ ���� ��ü ���� 
	if (DirectSoundCreate8(NULL, &g_lpDS, NULL) != DS_OK)
		return FALSE;

	//���¼��� ����- DSSCL_NORMAL�� ���� 
	if (g_lpDS->SetCooperativeLevel(hWnd, DSSCL_NORMAL) != DS_OK)
		return FALSE;

	return TRUE;
}

//�Լ��� : DeleteDirectSound() 
//����   : DirectSound ��ü�� �����Ѵ�.
void Sound::DeleteDirectSound()
{
	g_lpDS->Release();

	g_lpDS = NULL;
}

//�Լ��� : LoadWave() 
//����   : ���Ϸ� ���� wav������ �о� �޸𸮿� �ε��Ѵ�.
BOOL Sound::LoadWave(LPSTR lpFileName)
{

	HMMIO         hmmio;              //wave������ �ڵ�

	MMCKINFO      ckInRIFF, ckIn;  //�θ� ûũ , �ڽ� ûũ 
	PCMWAVEFORMAT   pcmWaveFormat;
	WAVEFORMATEX* pWaveFormat;

	//���̺� ������ ����, MMIO �ڵ��� ��´�.
	hmmio = mmioOpen(lpFileName, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if (hmmio == NULL)  return FALSE;

	//������ ���� ûũ�̸��� ����ϰ�, ���� ��ġ�� RIFFûũ���� WAVEûũ��

	//ã�� ��������.
	ckInRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if ((mmioDescend(hmmio, &ckInRIFF, NULL, MMIO_FINDRIFF)) != 0)
	{

		mmioClose(hmmio, 0);  //�����ϸ� �����ִ� ���̺������� �ݰ� ����(�� ���ش�.)
		return FALSE;
	}

	//������ ���� ûũ�̸��� ����ϰ�, ���� ��ġ�� WAVEûũ���� fmt ûũ�� ã�� ��������.
	//����: ��� ûũ�� 4���� �����ڵ带 ���� ������ t ������ ���鹮�ڰ� �ִ�.
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(hmmio, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{

		mmioClose(hmmio, 0);//�����ϸ� �����ִ� ���̺������� �ݰ� ����(�� ���ش�.)
		return FALSE;
	}

	//fmt ûũ���� wav���� ����(Format)�� �о� ���δ�.
	if (mmioRead(hmmio, (HPSTR)&pcmWaveFormat, sizeof(pcmWaveFormat))
		!= sizeof(pcmWaveFormat))
	{

		mmioClose(hmmio, 0);//�����ϸ� �����ִ� ���̺������� �ݰ� ����(�� ���ش�.)
		return FALSE;
	}

	//WAVEFORMATEX�� �޸𸮿� �Ҵ� 
	pWaveFormat = new WAVEFORMATEX;

	//PCMWAVEFORMAT�κ��� �����Ѵ�.
	memcpy(pWaveFormat, &pcmWaveFormat, sizeof(pcmWaveFormat));
	pWaveFormat->cbSize = 0;



	//fmt Chunk ���� �θ�ûũ�� WAVE Chunk�� �ö󰣴�.
	if (mmioAscend(hmmio, &ckIn, 0))
	{

		mmioClose(hmmio, 0);//�����ϸ� �����ִ� ���̺������� �ݰ� ����(�� ���ش�.)
		return FALSE;
	}

	//������ ���� ûũ�̸��� ����ϰ�, ���� ��ġ�� WAVEûũ���� data ûũ��

	//ã�� ��������.
	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(hmmio, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{

		mmioClose(hmmio, 0);//�����ϸ� �����ִ� ���̺������� �ݰ� ����(�� ���ش�.)
		return FALSE;
	}

	BYTE* pData = NULL;
	//data chunk ������ ��ŭ �޸� �Ҵ�
	pData = new BYTE[ckIn.cksize];
	//data chunk�� �ִ� ������ wave data�� �о� ���δ�. 
	mmioRead(hmmio, (LPSTR)pData, ckIn.cksize);

	//������� ������ wav�����б⿡ ������ ���̹Ƿ�, �����ִ� wav������ �ݴ´�. 
	mmioClose(hmmio, 0);

	// DSBUFFERDESC ����ü ������ ä���.
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRLDEFAULT | DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE;

	dsbd.dwBufferBytes = ckIn.cksize;

	dsbd.lpwfxFormat = pWaveFormat;

	//���� ������ ����
	if (g_lpDS->CreateSoundBuffer(&dsbd, &g_lpDSBG, NULL) != DS_OK)
		return FALSE;

	VOID* pBuff1 = NULL;  //���� ������ ù��° �����ּ�  
	VOID* pBuff2 = NULL;  //���� ������ �ι�° �����ּ� 
	DWORD dwLength;      //ù��° ����ũ��        
	DWORD dwLength2;     //�ι�° ����ũ��

	//���� ���ۿ� ������ wav�����͸� �����ϱ� ���� ���� �Ǵ�.
	if (g_lpDSBG->Lock(0, dsbd.dwBufferBytes, &pBuff1, &dwLength,
		&pBuff2, &dwLength2, 0L) != DS_OK)
	{

		g_lpDSBG->Release();

		g_lpDSBG = NULL;

		return FALSE;

	}

	memcpy(pBuff1, pData, dwLength);                     //������ ù��° ������ ����

	memcpy(pBuff2, (pData + dwLength), dwLength2); //������ �ι�° ������ ����

	//��� ���¸� Ǯ���ش�.
	g_lpDSBG->Unlock(pBuff1, dwLength, pBuff2, dwLength2);
	pBuff1 = pBuff2 = NULL;

	//�Ҵ�� �޸𸮸� ����
	delete[] pData;
	delete pWaveFormat;

	return TRUE;
}

//�Լ��� : Play()
//����   : �ش� ���带 �÷��� �Ѵ�.
void Sound::Play(BOOL Loop)
{
	//���۰� ��������� ���� 
	if (g_lpDSBG == NULL)  return;

	//����� �����ϸ� ���� 
	if (!g_lpDSBG->Play(0, 0, (Loop) ? 1 : 0))  return;

	g_bPlay = TRUE;
}

//�Լ��� : Stop()
//����   : �ش� ���带 �����.
void Sound::Stop()
{
	//���۰� ��������� ����

	if (g_lpDSBG == NULL)  return;

	g_lpDSBG->Stop();  //����

	g_bPlay = FALSE;

	g_lpDSBG->SetCurrentPosition(0L); //ó����ġ��
}

//�Լ��� : SetVolume()
//����   : �ش� ������ ������ �����Ѵ�.(100�̸� �ִ����, 0�̸� ����)
BOOL Sound::SetVolume(LONG lVolume)
{

	if (g_lpDSBG->SetVolume(DSVOLUME_TO_DB(lVolume)) != DS_OK)
		return FALSE;

	return TRUE;
}

//�Լ��� : SetVolume()
//����   : ���׷��� �д�����(������ -10000~10000)
BOOL Sound::SetPan(LONG lPan)
{

	if (g_lpDSBG->SetPan(lPan) != DS_OK)
		return FALSE;

	return TRUE;
}