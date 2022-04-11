#include <tchar.h>
#include "audio_manager.h"
#include "misc.h"

AudioManager::AudioManager()
{
	initialize_XAudio2();
	initialize_bgm();
	initialize_se();
}

AudioManager::~AudioManager()
{
	for (int i = 0; i < MAX_BGM_COUNT; ++i) {
		if (bgm_source_voices[i] != nullptr)
		{
			bgm_source_voices[i]->DestroyVoice();
			bgm_source_voices[i] = nullptr;
		}
		if (bgm_pdata_buffer[i] != nullptr) { delete bgm_pdata_buffer[i]; }
	}
	for (int i = 0; i < MAX_SE_COUNT; ++i) {
		if (se_source_voices[i] != nullptr)
		{
			se_source_voices[i]->DestroyVoice();
			se_source_voices[i] = nullptr;
		}
		if (se_pdata_buffer[i] != nullptr) { delete se_pdata_buffer[i]; }
	}
	if (pXAudio2 != nullptr)
	{
		pXAudio2->Release();
		pXAudio2 = nullptr;
	}
	CoUninitialize();
}

void AudioManager::initialize_XAudio2()
{
	HRESULT hr{ S_OK };
	// How to: Initialize XAudio2 https://docs.microsoft.com/en-us/windows/win32/xaudio2/how-to--initialize-xaudio2
	// Make sure you have initialized COM.
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	// if (FAILED(hr)) return false;
	// Use the XAudio2Create function to create an instance of the XAudio2 engine.
	hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	// Use the CreateMasteringVoice method to create a mastering voice.
	IXAudio2MasteringVoice* p_master_voice;
	hr = pXAudio2->CreateMasteringVoice(&p_master_voice);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

//-------------------------------------------------------
//      BGM
//-------------------------------------------------------
void AudioManager::initialize_bgm()
{
	register_bgm(L"resources/Audios/009.wav", BGM_INDEX::TITLE, 0.1f);
	register_bgm(L"resources/Audios/ending.wav", BGM_INDEX::ENDING, 0.1f);


}

void AudioManager::register_bgm(const LPCWSTR filename,
	BGM_INDEX index, float volume, bool is_loop)
{
	HRESULT hr{ S_OK };
	// Populating XAudio2 structures with the contents of RIFF chunks https://docs.microsoft.com/en-us/windows/win32/xaudio2/how-to--load-audio-data-files-in-xaudio2
    // Declare WAVEFORMATEXTENSIBLE and XAUDIO2_BUFFER structures.
	WAVEFORMATEXTENSIBLE wfx = { 0 };
	XAUDIO2_BUFFER buffer = { 0 };
	// Open the audio file with CreateFile.
	HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, 0, NULL);
	_ASSERT_EXPR(INVALID_HANDLE_VALUE != hFile, L"Cannot find audio file");
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	// Locate the 'RIFF' chunk in the audio file, and check the file type.
	DWORD dwChunkSize;
	DWORD dwChunkPosition;
	FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
	DWORD filetype;
	ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
	_ASSERT_EXPR(filetype == fourccWAVE, L"Check the file type, should be fourccWAVE or 'XWMA'");
	// Locate the 'fmt ' chunk, and copy its contents into a WAVEFORMATEXTENSIBLE structure.
	FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
	ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
	// Locate the 'data' chunk, and read its contents into a buffer.
	FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
	bgm_pdata_buffer[static_cast<int>(index)] = new BYTE[dwChunkSize];
	ReadChunkData(hFile, bgm_pdata_buffer[static_cast<int>(index)], dwChunkSize, dwChunkPosition);
	// Populate an XAUDIO2_BUFFER structure.
	buffer.AudioBytes = dwChunkSize;  //size of the audio buffer in bytes
	buffer.pAudioData = bgm_pdata_buffer[static_cast<int>(index)];  //buffer containing audio data
	buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
	if (is_loop) {
		// ���[�v�̐ݒ� https://docs.microsoft.com/ja-jp/windows/win32/api/xaudio2/ns-xaudio2-xaudio2_buffer?redirectedfrom=MSDN
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	// How to: Play a Sound with XAudio2 https://docs.microsoft.com/en-us/windows/win32/xaudio2/how-to--play-a-sound-with-xaudio2
	// Create a source voice by calling the IXAudio2::CreateSourceVoice method on an instance of the XAudio2 engine. The format of the voice is specified by the values set in a WAVEFORMATEX structure.
	hr = pXAudio2->CreateSourceVoice(&bgm_source_voices[static_cast<int>(index)], (WAVEFORMATEX*)&wfx);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	// 12 Submit an XAUDIO2_BUFFER to the source voice using the function SubmitSourceBuffer.
	hr = bgm_source_voices[static_cast<int>(index)]->SubmitSourceBuffer(&buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = bgm_source_voices[static_cast<int>(index)]->SetVolume(volume);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void AudioManager::play_bgm(BGM_INDEX index)
{
	HRESULT hr{ S_OK };
	assert(bgm_source_voices[static_cast<int>(index)] != nullptr
		&& "BGM���ݒ肳��Ă��܂���A�Đ�����BGM�̃C���f�b�N�X���ԈႦ�Ă���\��������܂�");
	// Use the Start function to start the source voice.
	hr = bgm_source_voices[static_cast<int>(index)]->Start(0);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void AudioManager::stop_bgm(BGM_INDEX index)
{
	HRESULT hr{ S_OK };
	// Use the Start function to start the source voice.
	hr = bgm_source_voices[static_cast<int>(index)]->Stop(0);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void AudioManager::set_volume_bgm(BGM_INDEX index, float volume)
{
	HRESULT hr{ S_OK };
	assert(bgm_source_voices[static_cast<int>(index)] != nullptr && "BGM���ݒ肳��Ă��܂���");
	hr = bgm_source_voices[static_cast<int>(index)]->SetVolume(volume);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

//-------------------------------------------------------
//      SE
//-------------------------------------------------------
void AudioManager::initialize_se()
{
	enum LOOP_COUNT
	{
		LOOP_INFINITY = -1,
		LOOP_NONE,

		LOOP_MAX = 254,
	};

	register_se(L"resources/Audios/decision.wav", SE_INDEX::DECISION, 0.1f);
	register_se(L"resources/Audios/item_get.wav", SE_INDEX::GET, 0.1f);

}

//-------------------------------------------------------------
// int loop_count       -1:�������[�v  0:���[�v�Ȃ�(�f�t�H���g)
// �ŏ��l:-1 �ő�l:254  ���̑�:�C�Ӊ񐔃��[�v
//-------------------------------------------------------------
void AudioManager::register_se(const LPCWSTR filename,
	SE_INDEX index, float volume, int loop_count)
{
	HRESULT hr{ S_OK };
	// Populating XAudio2 structures with the contents of RIFF chunks https://docs.microsoft.com/en-us/windows/win32/xaudio2/how-to--load-audio-data-files-in-xaudio2
	// Declare WAVEFORMATEXTENSIBLE and XAUDIO2_BUFFER structures.
	WAVEFORMATEXTENSIBLE wfx = { 0 };
	// Open the audio file with CreateFile.
	HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, 0, NULL);
	_ASSERT_EXPR(INVALID_HANDLE_VALUE != hFile, L"Cannot find audio file");
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	// Locate the 'RIFF' chunk in the audio file, and check the file type.
	DWORD dwChunkSize;
	DWORD dwChunkPosition;
	FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
	DWORD filetype;
	ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
	_ASSERT_EXPR(filetype == fourccWAVE, L"Check the file type, should be fourccWAVE or 'XWMA'");
	// Locate the 'fmt ' chunk, and copy its contents into a WAVEFORMATEXTENSIBLE structure.
	FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
	ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
	// Locate the 'data' chunk, and read its contents into a buffer.
	FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
	se_pdata_buffer[static_cast<int>(index)] = new BYTE[dwChunkSize];
	ReadChunkData(hFile, se_pdata_buffer[static_cast<int>(index)], dwChunkSize, dwChunkPosition);
	// Populate an XAUDIO2_BUFFER structure.
	buffer_se[static_cast<int>(index)].AudioBytes = dwChunkSize;  //size of the audio buffer in bytes
	buffer_se[static_cast<int>(index)].pAudioData = se_pdata_buffer[static_cast<int>(index)];  //buffer containing audio data
	buffer_se[static_cast<int>(index)].Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
	assert(loop_count >= -1 && "loop_count���L�蓾�Ȃ������ɂȂ��Ă��܂��Bloop_count��-1�ȏ�254�����̐����ɂ��Ă�������");
	loop_count = (std::min)(XAUDIO2_MAX_LOOP_COUNT, loop_count);
	if (loop_count == -1) {
		// ���[�v�̐ݒ� https://docs.microsoft.com/ja-jp/windows/win32/api/xaudio2/ns-xaudio2-xaudio2_buffer?redirectedfrom=MSDN
		// �������[�v
		buffer_se[static_cast<int>(index)].LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	else if (loop_count == 0) { /*���[�v�Ȃ�*/ buffer_se[static_cast<int>(index)].LoopBegin = 0; buffer_se[static_cast<int>(index)].LoopLength = 0; }
	else { /*�C�Ӊ񐔃��[�v*/ 	buffer_se[static_cast<int>(index)].LoopCount = loop_count; }
	// How to: Play a Sound with XAudio2 https://docs.microsoft.com/en-us/windows/win32/xaudio2/how-to--play-a-sound-with-xaudio2
	// Create a source voice by calling the IXAudio2::CreateSourceVoice method on an instance of the XAudio2 engine. The format of the voice is specified by the values set in a WAVEFORMATEX structure.
	hr = pXAudio2->CreateSourceVoice(&se_source_voices[static_cast<int>(index)], (WAVEFORMATEX*)&wfx);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	// Submit an XAUDIO2_BUFFER to the source voice using the function SubmitSourceBuffer.
	hr = se_source_voices[static_cast<int>(index)]->SubmitSourceBuffer(&buffer_se[static_cast<int>(index)]); // �o�b�t�@�ɓ]��
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = se_source_voices[static_cast<int>(index)]->SetVolume(volume);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void AudioManager::play_se(SE_INDEX index)
{
	HRESULT hr{ S_OK };
	assert(se_source_voices[static_cast<int>(index)] != nullptr
		&& "SE���ݒ肳��Ă��܂���A�Đ�����SE�̃C���f�b�N�X���ԈႦ�Ă���\��������܂�");
	// Use the Start function to start the source voice.
	hr = se_source_voices[static_cast<int>(index)]->Stop(0); // �ꎞ��~
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = se_source_voices[static_cast<int>(index)]->FlushSourceBuffers(); // �Ȃ��ŏ��ɖ߂�
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = se_source_voices[static_cast<int>(index)]->SubmitSourceBuffer(&buffer_se[static_cast<int>(index)]); // �o�b�t�@�ɓ]��
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = se_source_voices[static_cast<int>(index)]->Start(0); // �Đ�
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void AudioManager::set_volume_se(SE_INDEX index, float volume)
{
	HRESULT hr{ S_OK };
	assert(se_source_voices[static_cast<int>(index)] != nullptr	&& "SE���ݒ肳��Ă��܂���");
	hr = se_source_voices[static_cast<int>(index)]->SetVolume(volume);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}