import streamlit as st
import json
import os
import csv
import pandas as pd

# Configuração da página Web
st.set_page_config(page_title="IHM Alta Performance (ISA101)", layout="wide")

# Padrão ISA101: Cores sóbrias e neutras
COR_FUNDO = "#DCDCDC"
COR_PAINEL = "#F0F0F0"
COR_TEXTO = "#333333"
COR_BOMBA_LIGADA = "#555555"
COR_BOMBA_DESLIGADA = "#AAAAAA"
COR_ALERTA_FALHA = "#FF4500"
COR_FLUIDO = "#4A708B"
COR_FUNDO_TANQUE = "#E0E0E0"
COR_MANUTENCAO_ON = "#4682B4"  
COR_MANUTENCAO_OFF = "#8B8989" 

ARQUIVO_COMANDOS = "comandos_python.json"
ARQUIVO_CSV = "historico_estacao.csv"

# Injeta CSS global
st.markdown(f"""
    <style>
        .stApp {{ background-color: {COR_FUNDO}; color: {COR_TEXTO}; }}
        .block-container {{ padding-top: 2rem; }}
        h1 {{ text-align: center; color: {COR_TEXTO}; font-family: Arial, sans-serif; font-weight: bold; }}
        
        .bomba-container {{
            background-color: {COR_PAINEL}; 
            border: 2px solid #bbb; 
            padding: 15px; 
            border-radius: 5px; 
            text-align: center;
            margin-bottom: 15px;
        }}
        .bomba-titulo {{ margin: 0 0 10px 0; color: {COR_TEXTO}; }}
        
        div.stButton > button {{
            width: 100%;
            border: none;
            color: white !important;
            padding: 12px 0px;
            font-size: 16px;
            font-weight: bold;
            border-radius: 4px;
            cursor: pointer;
        }}
        div.stButton > button:hover {{ opacity: 0.85; color: white !important; }}
    </style>
""", unsafe_allow_html=True)

st.markdown("<h1>Supervisório de Alta Performance (Web) - EB-016</h1>", unsafe_allow_html=True)
st.divider()

def enviar_comando_bomba(tag_bomba, acao):
    dados_comando = { "bomba_comandos": [{"tag": tag_bomba, "comando": acao}]}
    try:
        with open(ARQUIVO_COMANDOS, "w") as f:
            json.dump(dados_comando, f, indent=4)
        st.toast(f"Comando {acao} enviado!", icon="🚀")
    except Exception as e:
        st.error(f"Falha ao enviar comando: {e}")

@st.fragment(run_every=1)
def renderizar_painel_dinamico():
    nivel_atual = 0.0
    nivel_maximo = 100.0
    estados_bombas = {
        "BMB-00": {"ligada": False, "falha": False},
        "BMB-01": {"ligada": False, "falha": False},
        "BMB-02": {"ligada": False, "falha": False},
        "BMB-03": {"ligada": False, "falha": False},
    }

    em_manutencao = False
    # --- LEITURA TEMPO REAL (Última Linha do CSV) ---
    if os.path.exists(ARQUIVO_CSV):
        try:
            with open(ARQUIVO_CSV, "r") as f:
                leitor = list(csv.reader(f, delimiter=';'))
                if len(leitor) >= 2:
                 ultima_linha = [celula.strip() for celula in leitor[-1]]
                for i in range(len(ultima_linha)):
                    
                    # 1. Captura de Nível
                    if "%" in ultima_linha[i]:
                        try: nivel_atual = float(ultima_linha[i - 1])
                        except ValueError: pass
                    
                    # 2. Captura de Manutenção vinda da Telemetria do C++
                    if "EB-016-NORMAL" in ultima_linha[i]:
                        try: 
                            # Converte a string "True"/"1" para booleano real do Python
                            valor_csv = ultima_linha[i + 1].strip()
                            em_manutencao = valor_csv in ["True", "true", "1"]
                        except IndexError: 
                            pass
                    
                    # 3. Captura do Estado das Bombas
                    if ultima_linha[i] in estados_bombas:
                        tag_encontrada = ultima_linha[i]
                        if i + 4 < len(ultima_linha):
                            estados_bombas[tag_encontrada]["ligada"] = (ultima_linha[i + 3] == "Ligada")
                            estados_bombas[tag_encontrada]["falha"] = (ultima_linha[i + 4] == "emFalha")
        except Exception:
            pass

    # --- 1. SEÇÃO DE CONTROLE GLOBAL (MANUTENÇÃO) ---
    st.markdown(f"### Controle do Sistema")
    cor_btn_manutencao = COR_MANUTENCAO_ON if em_manutencao else COR_MANUTENCAO_OFF
    texto_manutencao = "MANUTENÇÃO: ATIVA (True)" if em_manutencao else "MANUTENÇÃO: INATIVA (False)"
    st.markdown(f'<div style="background-color: {COR_PAINEL}; border: 2px solid #bbb; padding: 10px; border-radius: 5px; text-align: center; margin-bottom: 15px;"><span style="background-color: {cor_btn_manutencao}; color: white; padding: 6px 12px; border-radius: 4px; font-weight: bold;">{texto_manutencao}</span></div>', unsafe_allow_html=True)
    st.divider()

    # --- 2. SEÇÃO: 3 GRÁFICOS ALINHADOS NA MESMA LINHA (ESTÁTICOS ISA101 - ÚLTIMAS 10 LEITURAS) ---
    dados_nivel = []
    dados_pressao = []
    dados_vazao = []

    if os.path.exists(ARQUIVO_CSV):
        try:
            df = pd.read_csv(ARQUIVO_CSV, sep=';', header=None)
            for index, row_data in df.iterrows():
                lista_linha = list(row_data)
                for i in range(len(lista_linha)):
                    if str(lista_linha[i]).strip() == "%":
                        try: dados_nivel.append(float(lista_linha[i-1]))
                        except: pass
                    if "bar" in str(lista_linha[i]):
                        try: dados_pressao.append(float(lista_linha[i-1]))
                        except: pass
                    if "L/h" in str(lista_linha[i]):
                        try: dados_vazao.append(float(lista_linha[i-1]))
                        except: pass
            
            # Fallback caso C++ não envie pressão/vazão ainda
            if not dados_pressao and dados_nivel: dados_pressao = [n * 0.05 for n in dados_nivel]
            if not dados_vazao and dados_nivel: dados_vazao = [n * 1.2 for n in dados_nivel]

            # FILTRO CRUCIAL: Mantém apenas as últimas 10 leituras para máxima performance
            dados_nivel = dados_nivel[-10:]
            dados_pressao = dados_pressao[-10:]
            dados_vazao = dados_vazao[-10:]

        except Exception:
            pass

    # Garante que as listas tenham dados mínimos para não quebrar o plot
    if not dados_nivel: dados_nivel = [0] * 10
    if not dados_pressao: dados_pressao = [0] * 10
    if not dados_vazao: dados_vazao = [0] * 10

    # Importação local do matplotlib para renderização estática rápida
    import matplotlib.pyplot as plt

    # Função auxiliar para gerar o gráfico estático padrão ISA101 (Cinza, limpo, sem firulas)
    def gerar_grafico_isa101(dados, titulo, cor_linha):
        fig, ax = plt.subplots(figsize=(4, 1.2)) # Tamanho compacto
        fig.patch.set_facecolor(COR_PAINEL)      # Fundo idêntico ao painel
        ax.set_facecolor(COR_PAINEL)
        
        # Plota a linha sólida fina
        ax.plot(dados, color=cor_linha, linewidth=1.5, linestyle='-')
        
        # Estilização minimalista ISA101 (Remove bordas desnecessárias)
        ax.spines['top'].set_visible(False)
        ax.spines['right'].set_visible(False)
        ax.spines['left'].set_color('#888888')
        ax.spines['bottom'].set_color('#888888')
        
        # Configura textos e fontes discretas
        ax.set_title(titulo, fontsize=9, fontweight='bold', color=COR_TEXTO, pad=4)
        ax.tick_params(axis='both', colors='#555555', labelsize=7)
        ax.grid(True, linestyle=':', alpha=0.5, color='#888888') # Grid discreto pontilhado
        
        plt.tight_layout()
        return fig

    # Cria as 3 colunas coladas lado a lado para os gráficos
    col_g1, col_g2, col_g3 = st.columns(3)

    with col_g1:
        fig_p = gerar_grafico_isa101(dados_pressao, "PRESSÃO (bar) - ÚLTIMAS 10", "#555555")
        st.pyplot(fig_p, clear_figure=True)

    with col_g2:
        fig_n = gerar_grafico_isa101(dados_nivel, "NÍVEL (L) - ÚLTIMAS 10", COR_FLUIDO)
        st.pyplot(fig_n, clear_figure=True)

    with col_g3:
        fig_v = gerar_grafico_isa101(dados_vazao, "VAZÃO (L/h) - ÚLTIMAS 10", "#777777")
        st.pyplot(fig_v, clear_figure=True)

    st.divider()

    # --- 3. LAYOUT PRINCIPAL (TANQUE E BOMBAS) ---
    col_tanque, col_bombas = st.columns([1, 2])

    with col_tanque:
        st.markdown(f"### Nível do Tanque")
        nivel_clamp = max(0.0, min(nivel_atual, nivel_maximo))
        altura_fluido_percent = (nivel_clamp / nivel_maximo) * 100
        altura_vazio_percent = 100 - altura_fluido_percent
        st.markdown(f'<div style="background-color: {COR_PAINEL}; border: 2px solid #bbb; padding: 20px; border-radius: 5px; text-align: center;"><div style="height: 250px; width: 100px; background-color: {COR_FUNDO_TANQUE}; margin: 0 auto; border: 1px solid #999; display: flex; flex-direction: column; justify-content: flex-end;"><div style="height: {altura_vazio_percent}%; width: 100%;"></div><div style="height: {altura_fluido_percent}%; width: 100%; background-color: {COR_FLUIDO};"></div></div><h3 style="margin-top: 15px; color: {COR_TEXTO}; font-family: monospace;">{nivel_atual:.1f} L</h3></div>', unsafe_allow_html=True)

    with col_bombas:
        st.markdown("### Painel Interativo das Bombas")
        tags = list(estados_bombas.keys())
        for row in range(2):
            c1, c2 = st.columns(2)
            for col, idx in [(c1, row*2), (c2, row*2+1)]:
                if idx < len(tags):
                    tag = tags[idx]
                    dados = estados_bombas[tag]
                    if dados["ligada"]:
                        cor_status, texto_status, proxima_acao = COR_BOMBA_LIGADA, "LIGADA (Clique p/ Desligar)", "FORCAR_DESLIGAR"
                    else:
                        cor_status, texto_status, proxima_acao = COR_BOMBA_DESLIGADA, "DESLIGADA (Clique p/ Ligar)", "FORCAR_LIGAR"

                    with col:
                        st.markdown(f'<div class="bomba-container"><h4 class="bomba-titulo">Bomba {tag}</h4></div>', unsafe_allow_html=True)
                        st.markdown(f'<style>div.stButton > button[key="btn_{tag}"] {{ background-color: {cor_status} !important; margin-top: -35px; }}</style>', unsafe_allow_html=True)
                        st.button(texto_status, key=f"btn_{tag}", on_click=enviar_comando_bomba, args=(tag, proxima_acao), use_container_width=True)

# Executa o painel
renderizar_painel_dinamico()