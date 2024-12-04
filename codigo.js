import { createClient } from 'https://cdn.jsdelivr.net/npm/@supabase/supabase-js@2/+esm';

const supabase = createClient('https://fkobhwxaljobbnznmhih.supabase.co','eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImZrb2Jod3hhbGpvYmJuem5taGloIiwicm9sZSI6ImFub24iLCJpYXQiOjE3MzMyMzU5NDEsImV4cCI6MjA0ODgxMTk0MX0.u2f6_v84xO6ww7PJo3HaIlT35E3hGXxj0yNwVYkRRRk')

var id_digital = 1;

async function cadastrar(){
    var nome = document.getElementById("nome").value;
    var cpf = document.getElementById("cpf").value;
    var genero = document.getElementById("genero").value;
    console.log(nome);
    console.log(cpf);
    console.log(genero);

    if(nome && cpf && genero){
        const { error } = await supabase
            .from('Usuários')
            .insert({ Nome: nome,Genero: genero,Digital_number: id_digital,CPF: cpf,Cadastrado: 0 })
        if (error) {
            console.error("Erro ao inserir no banco:", error);
            document.getElementById("espera").value = "Erro ao cadastrar a Digital";
        } 
        else{
            var cadastrado = 0;
            while(cadastrado == 0){
                const{data} = await supabase
                .from('Usuários')
                .select('Cadastrado')
                .eq('Digital_number',id_digital)
                cadastrado = data[0].Cadastrado;
                //console.log(cadastrado);
            document.getElementById("espera").innerHTML = "Cadastrando Digital...";
            }
            document.getElementById("espera").innerHTML = "";
            console.log("Usuário cadastrado com sucesso!");
        }
    }else{
        window.alert("Insira todos os campos!")
    }

    id_digital+=1;

    document.getElementById("nome").value = "";
    document.getElementById("cpf").value = "";
    document.getElementById("genero").value = "";
}

async function leitura(){
// Define 'Ler' como true
const { error: updateError } = await supabase
.from('Leitura')
.update({ Ler: true })
.eq('id', 1); // Filtra a linha pela coluna 'id'

if (updateError) {
console.error("Erro ao atualizar 'Ler':", updateError);
return;
}
console.log("'Ler' atualizado para true. Aguardando 'Status' ser false...");
document.getElementById("espera_ler").innerHTML = "Lendo a Digital...";
// Aguarda o 'Status' se tornar false
let Ler = true;
let digital_id = 0;
while (Ler) {
const { data, error: fetchError } = await supabase
    .from('Leitura')
    .select('Ler')
    .eq('id', 1)
    .single(); // Retorna apenas a linha específica

if (fetchError) {
    console.error("Erro ao buscar 'Status':", fetchError);
    return;
}

Ler = data.Ler; // Atualiza o valor de 'status'

if (Ler) {
    console.log("Status ainda é true. Aguardando...");
    await new Promise(resolve => setTimeout(resolve, 1000)); // Aguarda 1 segundo antes de verificar novamente
}
}
document.getElementById("espera_ler").innerHTML = "";
console.log("'Status' agora é false. Continuando execução...");
const { data, error: fetchError } = await supabase
    .from('Leitura')
    .select('digital')
    .eq('id', 1)
    .single(); // Retorna apenas a linha específica
    digital_id = data.digital;
    console.log(digital_id);
if(fetchError){
    console.error("Erro ao buscar 'Status':", fetchError);
}else{
    const { data,error } = await supabase
    .from('Usuários')
    .select('*')
    .eq('Digital_number', digital_id)
    .single();
    if(error){
        console.error("Erro ao buscar 'Status':", error);
    }else{
        var nome_id = data.Nome;
        var cpf_id = data.CPF;
        var genero_id = data.Genero;
    
        document.getElementById("nome_usuario").value = nome_id;
        document.getElementById("cpf_usuario").value = cpf_id;
        document.getElementById("genero_usuario").value = genero_id;
        document.getElementById("Digital").value = digital_id;
        const {error} = await supabase
            .from('Leitura')
            .update({digital: 0})
            .eq('id',1);
    }
}
}

window.leitura = leitura;
window.cadastrar = cadastrar;