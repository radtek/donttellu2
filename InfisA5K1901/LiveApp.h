
#include <Windows.h>
#include <stdlib.h>

typedef struct {
	int x;				//	��������Ͻ�λ�ã����ͼ��
	int y;
	int beExist;		//	���ڱ�ǣ�0...�����ڣ�1...����
	int nAngle;			//	��ת�Ƕ�  ��ֱ������0�ȣ��ұ������Ƕȣ�����Ǹ��Ƕȣ�ֻ�Ƕ�ƽ����ӡָ������
	int nWidth;			//	������
	int nHeight;		//	���󳤶�
	int nObjectType;	//	�������ͣ�����������������ȸ�ֵ����������б�ʾ��������ݵ����ͣ���������б�ʾϣ�����صĶ�������
} REGIONRECT;

//�ж�ƽ��ָ����ӡ����ͷ��������ӡ�����޷��жϣ�
//��������ֵ,    1....		�жϳɹ�����pDestObj�ṹ�з��زɼ�������Ͻ�λ�ú�ָ�Ʒ��򣨴�ֱ����Ϊ0�ȣ��Ƕ���������
//				-1....		��ӡ�����޷��ж�ʱ�򣬸�����ӡ�����������pDestObj�з��زɼ�������Ͻ�λ��
//				KERROR_XXX..��������
//���ú���֮ǰ����Ҫ��pDestObj�и��ɼ����Ⱥ͸߶ȸ�ֵ
//pDestObj��ָλ��ϢnObjectType ��ֵ21...30, -1��ʾָλδ֪
extern int PlainImage_Splite(unsigned char *pImage, int nWidth, int nHeight, REGIONRECT *pDestObj);

//�ж�ָ������ӡ������Ϣ
//��������ֵ��	>=0  ����Ĵ�С
//				< 0  ������Ϣ KERROR_***
extern int FPImage_AreaInfo(unsigned char *pImage,	//	�����ͼ��ָ��
						 int nWidth,				//	ͼ��Ŀ��
						 int nHeight,				//	ͼ��ĸ߶�
						 REGIONRECT *pDestObj);		//	���ص�ͼ����Ӿ�����Ϣ�����Ͻ�λ�ú;��εĿ�ȸ߶�